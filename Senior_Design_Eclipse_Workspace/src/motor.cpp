#include "motor.h"
#include "stdlib.h"



//----------------------------------------------------------------------------//
Motor::Motor(void) :
		motor_id_(Azimuthal_Motor),
		duty_(0),
		dir_(false),
		prescaler_(99),
		period_(99),
		enableA_(false),
		enableV_(false),
		enableC_(false),
		counts_to_position_(0.0),
		overflows_(0),
		prev_counter_(0),
		encoder_bit_A(GPIO_PIN_RESET),
		encoder_bit_B(GPIO_PIN_RESET)
{

	/* Initalization of GPIO pin to control the motor's direction
	   through the direction pin of the h-bridge  */
	GPIO_InitTypeDef GPIO_InitStruct;

	// Enable GPIO Ports
	__HAL_RCC_GPIOI_CLK_ENABLE();

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

	GPIO_InitStruct.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
}

//----------------------------------------------------------------------------//
Motor::Motor(TIM_TypeDef *TIMX, const float counts_to_position, const int encoder_bits) :
		motor_id_(Azimuthal_Motor),
		duty_(0),
		dir_(false),
		prescaler_(99),
		period_(99),
		enableA_(false),
		enableV_(false),
		enableC_(false),
		counts_to_position_(counts_to_position),
		overflows_(0),
		prev_counter_(0)
{

	/* Initalization of GPIO pin to control the motor's direction
	   through the direction pin of the h-bridge  */
	GPIO_InitTypeDef GPIO_InitStruct;

	// Enable GPIO Ports
	__HAL_RCC_GPIOI_CLK_ENABLE();

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

	GPIO_InitStruct.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

	// Initialization stuff
	TIM_HANDLE_.Instance = TIMX;

	// 1 - Configure the TIM peripheral
	TIM_HANDLE_.Init.Prescaler = prescaler_;
	TIM_HANDLE_.Init.Period = period_;
	TIM_HANDLE_.Init.ClockDivision = 0;
	TIM_HANDLE_.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIM_HANDLE_.Init.RepetitionCounter = 0;
	if (HAL_TIM_PWM_Init(&TIM_HANDLE_) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}

	// 2 - Configure the PWM channels
	/* Common configuration for all channels */
	sMotorConfig.OCMode = TIM_OCMODE_PWM1;
	sMotorConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
	sMotorConfig.OCFastMode = TIM_OCFAST_DISABLE;
	sMotorConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sMotorConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	sMotorConfig.OCIdleState = TIM_OCIDLESTATE_RESET;

	// 3 - Start PWM signals generation
	/* Start channel 1 */
	sMotorConfig.Pulse = 0;
	HAL_TIM_PWM_Start(&TIM_HANDLE_, TIM_CHANNEL_1);

	if (encoder_bits % 2)
		encoder_bit_A = GPIO_PIN_SET;
	if ((encoder_bits/2) % 2)
		encoder_bit_B = GPIO_PIN_SET;
}

//----------------------------------------------------------------------------//
void Motor::motorInit(motor_id_t id)
{
	switch (id)
	{
	case Azimuthal_Motor:
		TIM_HANDLE_.Instance = TIM10;
		break;
	case Vertical_Motor:
		TIM_HANDLE_.Instance = TIM11;
		break;
	case Claw_Motor:
		TIM_HANDLE_.Instance = TIM13;
		break;
	}

	/*##-1- Configure the TIM peripheral #######################################*/
	TIM_HANDLE_.Init.Prescaler = prescaler_;
	TIM_HANDLE_.Init.Period = period_;
	TIM_HANDLE_.Init.ClockDivision = 0;
	TIM_HANDLE_.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIM_HANDLE_.Init.RepetitionCounter = 0;
	if (HAL_TIM_PWM_Init(&TIM_HANDLE_) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}

	/*##-2- Configure the PWM channels #########################################*/
	/* Common configuration for all channels */
	sMotorConfig.OCMode = TIM_OCMODE_PWM1;
	sMotorConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
	sMotorConfig.OCFastMode = TIM_OCFAST_DISABLE;
	sMotorConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sMotorConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	sMotorConfig.OCIdleState = TIM_OCIDLESTATE_RESET;

	/*##-3- Start PWM signals generation #######################################*/
	/* Start channel 1 */
	sMotorConfig.Pulse = 0;
	HAL_TIM_PWM_Start(&TIM_HANDLE_, TIM_CHANNEL_1);
}

//----------------------------------------------------------------------------//
void Motor::setEnable(motor_id_t id, bool enable)
{
	switch (id)
	{
	case Azimuthal_Motor:
		enableA_ = enable;
		enableV_ = false;
		enableC_ = false;
		break;
	case Vertical_Motor:
		enableV_ = enable;
		enableA_ = false;
		enableC_ = false;
		break;
	case Claw_Motor:
		enableC_ = enable;
		enableA_ = false;
		enableV_ = false;
		break;
	}
}

//----------------------------------------------------------------------------//
void Motor::setEnable()
{
	// 3 unique pairs of A and B define which encoder is fed through
	// the multiplexer to the STM32.
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_0, encoder_bit_A);
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, encoder_bit_B);
}

//----------------------------------------------------------------------------//
bool Motor::getEnable(motor_id_t id)
{
	switch (id)
	{
	case Azimuthal_Motor:
		return enableA_;
		break;
	case Vertical_Motor:
		return enableV_;
		break;
	case Claw_Motor:
		return enableC_;
		break;
	default:
		return 0;
	}
}

//----------------------------------------------------------------------------//
void Motor::setDuty(motor_id_t id, int16_t dutyInput)
{
	//	duty_ = dutyInput;
	switch (id)
	{
	case Azimuthal_Motor:
		TIM_HANDLE_.Instance = TIM10;
		break;
	case Vertical_Motor:
		TIM_HANDLE_.Instance = TIM11;
		break;
	case Claw_Motor:
		TIM_HANDLE_.Instance = TIM13;
		break;
	}

	/* Overflow protection of duty cycle. Duty cycle cannot be
	 * higher than the timer period length*/
	int16_t maxLimit = period_ + 1;
	if (dutyInput < -maxLimit)
		duty_ = -maxLimit;
	else if (dutyInput > maxLimit)
		duty_ = maxLimit;
	else
		duty_ = dutyInput;

	/*Sets motor direction depending on if the duty command is negative or positive*/
	if (duty_ < 0)
		setDirection(false);
	else
		setDirection(true);

	/* Set the pulse value for channel 1 */
	sMotorConfig.Pulse = abs(duty_);
	HAL_TIM_PWM_ConfigChannel(&TIM_HANDLE_, &sMotorConfig, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&TIM_HANDLE_, TIM_CHANNEL_1);
}

//----------------------------------------------------------------------------//
void Motor::setDuty(int16_t dutyInput)
{
	/* Overflow protection of duty cycle. Duty cycle cannot be
	 * higher than the timer period length*/
	int16_t maxLimit = period_ + 1;
	if (dutyInput < -maxLimit)
		duty_ = -maxLimit;
	else if (dutyInput > maxLimit)
		duty_ = maxLimit;
	else
		duty_ = dutyInput;

	/*Sets motor direction depending on if the duty command is negative or positive*/
	if (duty_ < 0)
		setDirection(false);
	else
		setDirection(true);

	/* Set the pulse value for channel 1 */
	sMotorConfig.Pulse = abs(duty_);
	HAL_TIM_PWM_ConfigChannel(&TIM_HANDLE_, &sMotorConfig, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&TIM_HANDLE_, TIM_CHANNEL_1);
}

//----------------------------------------------------------------------------//
void Motor::setDirection(bool direction)
{
	dir_ = direction;
	if (dir_)
		HAL_GPIO_WritePin(GPIOI, GPIO_PIN_3, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOI, GPIO_PIN_3, GPIO_PIN_SET);
}

//----------------------------------------------------------------------------//
bool Motor::getDirection(void)
{
	return (dir_);
	// jar: do these need separate values?
	//return (__HAL_TIM_IS_TIM_COUNTING_DOWN(&Encoder_Handle));
}


//----------------------------------------------------------------------------//
void Motor::Error_Handler(void)
{
	/* Turn LED3 on */
	BSP_LED_On(LED1);
	while (1)
	{
	}
}

//----------------------------------------------------------------------------//
int32_t Motor::getCount(void)
{
	uint16_t counter; // 16 bit counter of timer
	int32_t count32;  // 32 bit counter which accounts for timer overflows

	counter = __HAL_TIM_GET_COUNTER(&Encoder_Handle);

	// The following assumes this function is called frequently enough that
	// the encoder cannot change more 0x8000 counts between calls, and that
	// the counter overflows from 0xffff to 0 and underflows from 0 to 0xffff
	if ((prev_counter_ > 0xc000) && (counter < 0x4000))
	{
		overflows_ += 1; // overflow
	}
	else if ((prev_counter_ < 0x4000) && (counter > 0xc000))
	{
		overflows_ -= 1; // underflow
	}

	count32 = overflows_ * 0x10000 + counter;
	prev_counter_ = counter;

	return count32;
}

//----------------------------------------------------------------------------//
void Motor::setCount(int32_t count32)
{
	if (count32 < 0)
	{
		overflows_ = count32 / ((int32_t) 0x10000) - 1;
	}
	else
	{
		overflows_ = count32 / ((int32_t) 0x10000);
	}

	uint16_t counter = (uint16_t) (count32 - overflows_ * 0x10000);
	prev_counter_ = counter;

	__HAL_TIM_SET_COUNTER(&Encoder_Handle, counter);
}

//----------------------------------------------------------------------------//
float Motor::getPosition()
{
	// either set or ensure that we're set to the right encoder
	return getCount() * counts_to_position_;
}

//----------------------------------------------------------------------------//
float Motor::getDesiredPosition()
{
	return desiredPos_;
}

//----------------------------------------------------------------------------//
void Motor::setDesiredPosition(float desiredPos)
{
	desiredPos_ = desiredPos;
}

//----------------------------------------------------------------------------//
float Motor::getPosError()
{
	return getPosition() - getDesiredPosition();
}
