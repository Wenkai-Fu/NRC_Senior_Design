#include "encoder.h"

/*
 *  JAR notes
 *  It seems that __HAL_TIM_GET_COUNTER and __HAL_TIM_SET_COUNTER
 *  must be used because there is only one interface to the three
 *  encoders.
 *
 */

/* Timer handler declaration */
TIM_HandleTypeDef Encoder_Handle;
/* Timer Encoder Configuration Structure declaration */
TIM_Encoder_InitTypeDef sEncoderConfig;

//----------------------------------------------------------------------------//
Encoder::Encoder(void) :
		prev_counter_(0),
		overflows_(0),
		pos_(0),
		azimPos_(0),
		vertPos_(0),
		desiredAzimPos_(0),
		desiredVertPos_(0),
		desiredClawPos_(0),
		posError_(0)
{

	/* Initialization of GPIO pins responsable for encoder
	   selection with hardware multiplexer  */

	GPIO_InitTypeDef GPIO_InitStruct;

	// Enable GPIO Ports
	__HAL_RCC_GPIOI_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	/* -1- Initialize TIM1 to handle the encoder sensor */
	/* Initialize TIM1 peripheral as follows:
	 + Period = 65535
	 + Prescaler = 0
	 + ClockDivision = 0
	 + Counter direction = Up
	 */
	Encoder_Handle.Instance = TIM8;

	Encoder_Handle.Init.Period = 65535;
	Encoder_Handle.Init.Prescaler = 0;
	Encoder_Handle.Init.ClockDivision = 0;
	Encoder_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	Encoder_Handle.Init.RepetitionCounter = 0;

	sEncoderConfig.EncoderMode = TIM_ENCODERMODE_TI12;

	sEncoderConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
	sEncoderConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sEncoderConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sEncoderConfig.IC1Filter = 0;

	sEncoderConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
	sEncoderConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sEncoderConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sEncoderConfig.IC2Filter = 0;

	if (HAL_TIM_Encoder_Init(&Encoder_Handle, &sEncoderConfig) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}

	/* Start the encoder interface */
	HAL_TIM_Encoder_Start(&Encoder_Handle, TIM_CHANNEL_ALL);
}

//----------------------------------------------------------------------------//
uint16_t Encoder::getCount(void)
{
	uint16_t counter;
	counter = __HAL_TIM_GET_COUNTER(&Encoder_Handle);
	return counter;
}

//----------------------------------------------------------------------------//
void Encoder::setCount(uint16_t counter)
{
	__HAL_TIM_SET_COUNTER(&Encoder_Handle, counter);
}

//
////----------------------------------------------------------------------------//
//bool Encoder::getDirection(void)
//{
//	/* Get the current direction */
//	return (__HAL_TIM_IS_TIM_COUNTING_DOWN(&Encoder_Handle));
//}
//
//
//
////----------------------------------------------------------------------------//
//void Encoder::enableEncoder(encoder_id_t encode)
//{
//	// the binary combinations of the pin commands eg set give 3 unique values
//	switch (encode)
//	{
//	case Azimuthal_Encoder:
//		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET);  // Channel B
//		HAL_GPIO_WritePin(GPIOI, GPIO_PIN_0, GPIO_PIN_SET);    // Channel A
//		break;
//	case Vertical_Encoder:
//		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);    // Channel B
//		HAL_GPIO_WritePin(GPIOI, GPIO_PIN_0, GPIO_PIN_RESET);  // Channel A
//		break;
//	case Claw_Encoder:
//		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);    // Channel B
//		HAL_GPIO_WritePin(GPIOI, GPIO_PIN_0, GPIO_PIN_SET);    // Channel A
//		break;
//	}
//}
//
//----------------------------------------------------------------------------//
void Encoder::Error_Handler(void)
{
	/* Turn LED3 on */
	BSP_LED_On(LED1);
	while (1)
	{
	}
}
