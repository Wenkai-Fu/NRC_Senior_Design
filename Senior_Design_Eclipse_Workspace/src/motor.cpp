#include "motor.h"
#include "stdlib.h"

//----------------------------------------------------------------------------//
Motor::Motor(TIM_TypeDef *TIMX,
		     const float counts_to_position,
			 const int encoder_bits,
			 const float increment) :
		duty_(0),
		dir_(false),
		prescaler_(99),
		period_(99),
		counts_to_position_(counts_to_position),
		counter32_(0),
		overflows_(0),
		counter16_(0),
		increment_(increment)
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
void Motor::setEnable()
{
	// connect to the correct encoder. the 3 unique pairs of A and B define
	// which encoder is fed through the multiplexer to the STM32.
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_0, encoder_bit_A);
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, encoder_bit_B);

	// set the count to the last saved count.
	setCount();
}

//----------------------------------------------------------------------------//
void Motor::setDesiredPosition(float desiredPos)
{
	desiredPos_ = desiredPos;
}

//----------------------------------------------------------------------------//
void Motor::setDuty(int16_t dutyInput)
{
	// Overflow protection of duty cycle. Duty cycle cannot be
	// higher than the timer period length
	int16_t maxLimit = period_ + 1;
	if (dutyInput < -maxLimit)
		duty_ = -maxLimit;
	else if (dutyInput > maxLimit)
		duty_ = maxLimit;
	else
		duty_ = dutyInput;

	// Sets motor direction depending on if the duty is negative or positive
	if (duty_ < 0)
	{
		HAL_GPIO_WritePin(GPIOI, GPIO_PIN_3, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOI, GPIO_PIN_3, GPIO_PIN_RESET);
	}
	// __HAL_TIM_IS_TIM_COUNTING_DOWN(&Encoder_Handle) should be consistent
	// with the direction

	/* Set the pulse value for channel 1 */
	sMotorConfig.Pulse = abs(duty_);
	HAL_TIM_PWM_ConfigChannel(&TIM_HANDLE_, &sMotorConfig, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&TIM_HANDLE_, TIM_CHANNEL_1);
}

//----------------------------------------------------------------------------//
bool Motor::getDirection(void)
{
	return duty_ > 0;
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
float Motor::getPosError()
{
	return getPosition() - getDesiredPosition();
}

//----------------------------------------------------------------------------//
bool Motor::increase()
{
	// TODO: may want software limits.
	//if (desiredPos_ + increment_ > maxPos_) return false;
	desiredPos_ += increment_;
	return true;
}

//----------------------------------------------------------------------------//
bool Motor::decrease()
{
	//if (desiredPos_ - increment_ < minPos_) return false;
	desiredPos_ -= increment_;
	return true;
}


//============================================================================//
// PRIVATE
//============================================================================//

//----------------------------------------------------------------------------//
void Motor::Error_Handler(void)
{
	BSP_LED_On(LED1);
	while (1)
	{
		/* ... */
	}
}

//----------------------------------------------------------------------------//
int32_t Motor::getCount()
{
	// 16-bit counter of timer
	uint16_t counter = __HAL_TIM_GET_COUNTER(&Encoder_Handle);
	// The following assumes this function is called frequently enough that
	// the encoder cannot change more 0x8000 counts between calls, and that
	// the counter overflows from 0xffff to 0 and underflows from 0 to 0xffff
	if ((counter16_ > 0xc000) && (counter < 0x4000))
		overflows_ += 1; // overflow
	else if ((counter16_ < 0x4000) && (counter > 0xc000))
		overflows_ -= 1; // underflow
	counter16_ = counter;
	counter32_ = overflows_ * 0x10000 + counter;
	return counter32_;
}

//----------------------------------------------------------------------------//
void Motor::setCount()
{
	overflows_ = counter32_ / ((int32_t) 0x10000);
	if (counter32_ < 0) overflows_--;
	counter16_ = (uint16_t) (counter32_ - overflows_ * 0x10000);
	__HAL_TIM_SET_COUNTER(&Encoder_Handle, counter16_);
}
