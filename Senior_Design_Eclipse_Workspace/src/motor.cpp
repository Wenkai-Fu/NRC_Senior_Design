#include "stdlib.h"
#include "stm32f7xx_hal.h"
#include "stm32746g_discovery.h"
#include "motor.h"
#include "main.h"

/* Timer handler declaration*/
TIM_HandleTypeDef    TimHandle_TIM10;
TIM_HandleTypeDef    TimHandle_TIM11;
TIM_HandleTypeDef    TimHandle_TIM13;

/* Timer Output Compare Configuration Structure declaration */
TIM_OC_InitTypeDef sMotorConfig;

/* GPIO Pins for Motor declaration */
GPIO_InitTypeDef   GPIO_InitStruct;

/* Using the encoder class to enable encoders when motors are enabled*/
extern Encoder encoder;

Motor::Motor(void)
	:motor_id_(Azimuthal_Motor),
	 duty_ (0),
	 TIM_HANDLE_ (TimHandle_TIM10),
	 dir_ (false),
	 prescaler_ (99),
	 period_ (99),
	 enableA_ (false),
	 enableV_ (false),
	 enableC_ (false)
{

	/*Initalization of GPIO pin to control the motor's direction
	through the direction pin of the h-bridge*/
	GPIO_InitTypeDef GPIO_InitStruct;

	// Enable GPIO Ports
	__HAL_RCC_GPIOI_CLK_ENABLE();

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

	GPIO_InitStruct.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
}
void Motor::motorInit(motor_id_t id)
{
	switch (id)
	{
	case Azimuthal_Motor:
		TIM_HANDLE_ = TimHandle_TIM10;
		TIM_HANDLE_.Instance = TIM10;
		break;
	case Vertical_Motor:
		TIM_HANDLE_ = TimHandle_TIM11;
		TIM_HANDLE_.Instance = TIM11;
		break;
	case Claw_Motor:
		TIM_HANDLE_ = TimHandle_TIM13;
		TIM_HANDLE_.Instance = TIM13;
		break;
	}

	/*##-1- Configure the TIM peripheral #######################################*/
	TIM_HANDLE_.Init.Prescaler         = prescaler_;
	TIM_HANDLE_.Init.Period            = period_;
	TIM_HANDLE_.Init.ClockDivision     = 0;
	TIM_HANDLE_.Init.CounterMode       = TIM_COUNTERMODE_UP;
	TIM_HANDLE_.Init.RepetitionCounter = 0;
	if (HAL_TIM_PWM_Init(&TIM_HANDLE_) != HAL_OK)
	{
	/* Initialization Error */
	Error_Handler();
	}

	/*##-2- Configure the PWM channels #########################################*/
	/* Common configuration for all channels */
	sMotorConfig.OCMode       = TIM_OCMODE_PWM1;
	sMotorConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
	sMotorConfig.OCFastMode   = TIM_OCFAST_DISABLE;
	sMotorConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
	sMotorConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	sMotorConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

	/*##-3- Start PWM signals generation #######################################*/
	/* Start channel 1 */
	sMotorConfig.Pulse = 0;
	HAL_TIM_PWM_Start(&TIM_HANDLE_, TIM_CHANNEL_1);
}

void Motor::start(motor_id_t id)
{
	switch (id)
	{
	case Azimuthal_Motor:
		TIM_HANDLE_ = TimHandle_TIM10;
		TIM_HANDLE_.Instance = TIM10;
		break;
	case Vertical_Motor:
		TIM_HANDLE_ = TimHandle_TIM11;
		TIM_HANDLE_.Instance = TIM11;
		break;
	case Claw_Motor:
		TIM_HANDLE_ = TimHandle_TIM13;
		TIM_HANDLE_.Instance = TIM13;
		break;
	}
	/*##-3- Start PWM signals generation #######################################*/
	/* Start channel 1 */
	HAL_TIM_PWM_Start(&TIM_HANDLE_, TIM_CHANNEL_1);
}

void Motor::stop(motor_id_t id)
{
	switch (id)
	{
	case Azimuthal_Motor:
		TIM_HANDLE_ = TimHandle_TIM10;
		TIM_HANDLE_.Instance = TIM10;
		break;
	case Vertical_Motor:
		TIM_HANDLE_ = TimHandle_TIM11;
		TIM_HANDLE_.Instance = TIM11;
		break;
	case Claw_Motor:
		TIM_HANDLE_ = TimHandle_TIM13;
		TIM_HANDLE_.Instance = TIM13;
		break;
	}
	/*##-3- Start PWM signals generation #######################################*/
	/* Start channel 1 */
	HAL_TIM_PWM_Stop(&TIM_HANDLE_, TIM_CHANNEL_1);
}
void Motor::setEnable(motor_id_t id, bool enable)
{
	switch(id)
	{
	case Azimuthal_Motor:
		enableA_ = enable;
		//encoder.enableEncoder(Azimuthal_Encoder);
		enableV_ = false;
		enableC_ = false;
		break;
	case Vertical_Motor:
		enableV_ = enable;
		//encoder.enableEncoder(Vertical_Encoder);
		enableA_ = false;
		enableC_ = false;
		break;
	case Claw_Motor:
		enableC_ = enable;
		//encoder.enableEncoder(Claw_Encoder);
		enableA_ = false;
		enableV_ = false;
		break;
	}
}

bool Motor::getEnable(motor_id_t id)
{
	switch(id)
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
void Motor::setDuty(motor_id_t id, int16_t dutyInput)
{
//	duty_ = dutyInput;
	switch (id)
	{
	case Azimuthal_Motor:
		TIM_HANDLE_ = TimHandle_TIM10;
		TIM_HANDLE_.Instance = TIM10;
		break;
	case Vertical_Motor:
		TIM_HANDLE_ = TimHandle_TIM11;
		TIM_HANDLE_.Instance = TIM11;
		break;
	case Claw_Motor:
		TIM_HANDLE_ = TimHandle_TIM13;
		TIM_HANDLE_.Instance = TIM13;
		break;
	}

	/*Overflow protection of duty cycle. Duty cycle cannot be higher than the timer period length*/
	int16_t maxLimit = period_ + 1;
	if(dutyInput < -maxLimit) duty_ = -maxLimit;
	else if (dutyInput > maxLimit) duty_ = maxLimit;
	else duty_ = dutyInput;

	/*Sets motor direction depending on if the duty command is negative or positive*/
	if (duty_ < 0) setDirection(false);
	else setDirection(true);

	/* Set the pulse value for channel 1 */
	sMotorConfig.Pulse = abs(duty_);
	HAL_TIM_PWM_ConfigChannel(&TIM_HANDLE_, &sMotorConfig, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&TIM_HANDLE_, TIM_CHANNEL_1);
}

int16_t Motor::getDuty(motor_id_t id){return duty_;}

void Motor::setDirection(bool direction)
{
	dir_ = direction;
	if(dir_) HAL_GPIO_WritePin(GPIOI, GPIO_PIN_3, GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(GPIOI, GPIO_PIN_3, GPIO_PIN_SET);
}

bool Motor::getDirection(void){return(dir_);}

void Motor::Error_Handler(void)
{
	/* Turn LED3 on */
	BSP_LED_On(LED1);
	while(1)
	{
	}
}
