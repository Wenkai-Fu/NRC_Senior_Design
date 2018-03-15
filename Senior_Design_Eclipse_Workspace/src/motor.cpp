#include "motor.h"
#include "stdlib.h"
#include "main.h"

//----------------------------------------------------------------------------//
Motor::Motor(TIM_TypeDef *TIMX,
		     const float counts_to_position,
			 const int encoder_bits,
			 const float increment):
		duty_(0),
		dir_(false),
		prescaler_(99),
		period_(99),
		counts_to_position_(counts_to_position),
		overflows_(0),
		cnt(0),
		old_cnt(0),
		increment_(increment),
		desiredPos_(0.0),
		enable_(false),
		bot_ls(false),
		top_ls(false),
		claw_ls(false),
		id(encoder_bits),
		cor_establish(false)
{
	/* Initalization of GPIO pin to control the motor's direction
	   through the direction pin of the h-bridge  */
	GPIO_InitTypeDef GPIO_InitStruct;

	/* -1- Initialize TIM1 to handle the encoder sensor */
	  /* Initialize TIM1 peripheral as follows:
	       + Period = 65535
	       + Prescaler = 0
	       + ClockDivision = 0
	       + Counter direction = Up
	  */
	  Encoder_Handle.Instance = TIM8;

	  Encoder_Handle.Init.Period            = 65535;
	  Encoder_Handle.Init.Prescaler         = 0;
	  Encoder_Handle.Init.ClockDivision     = 0;
	  Encoder_Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	  Encoder_Handle.Init.RepetitionCounter = 0;

	  sEncoderConfig.EncoderMode        = TIM_ENCODERMODE_TI12;

	  sEncoderConfig.IC1Polarity        = TIM_ICPOLARITY_RISING;
	  sEncoderConfig.IC1Selection       = TIM_ICSELECTION_DIRECTTI;
	  sEncoderConfig.IC1Prescaler       = TIM_ICPSC_DIV1;
	  sEncoderConfig.IC1Filter          = 0;

	  sEncoderConfig.IC2Polarity        = TIM_ICPOLARITY_RISING;
	  sEncoderConfig.IC2Selection       = TIM_ICSELECTION_DIRECTTI;
	  sEncoderConfig.IC2Prescaler       = TIM_ICPSC_DIV1;
	  sEncoderConfig.IC2Filter          = 0;

	  if(HAL_TIM_Encoder_Init(&Encoder_Handle, &sEncoderConfig) != HAL_OK)
	  {
	    /* Initialization Error */
	    Error_Handler();
	  }

	/* Start the encoder interface */
	HAL_TIM_Encoder_Start(&Encoder_Handle, TIM_CHANNEL_ALL); 

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

	encoder_bit_A = GPIO_PIN_RESET;
	encoder_bit_B = GPIO_PIN_RESET;

	if (encoder_bits % 2)
		encoder_bit_A = GPIO_PIN_SET;
	if ((encoder_bits/2) % 2)
		encoder_bit_B = GPIO_PIN_SET;

	// set the count to the last saved count.
	setCount();

	if (id == 3 or id == 2)
		// home position to trigger switch
		desiredPos_ = -2000.0;
}

//----------------------------------------------------------------------------//
void Motor::enable()
{

	enable_ = true;
	// connect to the correct encoder. the 3 unique pairs of A and B define
	// which encoder is fed through the multiplexer to the STM32.
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, encoder_bit_B);
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_0, encoder_bit_A);

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
	// effective after coordinate system established
	desiredPos_ += increment_;
	if (id == 2){
		// z motor, bottom limit protection
		if (desiredPos_ >= 55.0)
			desiredPos_ = 55.0;
	}
	else if (id == 3){
		// claw motor, close protection
		if (desiredPos_ >= 5.0)
			desiredPos_ = 5.0;
	}
	return true;
}

//----------------------------------------------------------------------------//
bool Motor::decrease()
{
	// effective after coordinate system established
	desiredPos_ -= increment_;
	return true;
}

void Motor::set_zero(){
	desiredPos_ = 0.0;
	cnt = 0;
	old_cnt = 0;
	overflows_ = 0;
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
	old_cnt = cnt;
	cnt = __HAL_TIM_GET_COUNTER(&Encoder_Handle);
	int diff = cnt - old_cnt;
	// step change
	if (diff < -10000)
		overflows_ += 1;
	else if (diff > 10000)
		overflows_ -= 1;

	int32_t ans = overflows_ * 65536 + cnt;
	return ans;
}

uint32_t Motor::get_raw_count()
{
	return __HAL_TIM_GET_COUNTER(&Encoder_Handle);
}

//----------------------------------------------------------------------------//
void Motor::setCount()
{
	__HAL_TIM_SET_COUNTER(&Encoder_Handle, cnt);
}
