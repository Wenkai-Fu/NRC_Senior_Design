/*
 * encoder.cpp
 *
 *  Created on: Feb 29, 2016
 *      Author: smith
 */

#include "main.h"

	/* Timer handler declaration */
	TIM_HandleTypeDef    Encoder_Handle;
	/* Timer Encoder Configuration Structure declaration */
	TIM_Encoder_InitTypeDef sEncoderConfig;


Encoder::Encoder(void)
{
	prev_counter_ = 0;
	overflows_ = 0;


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

}

int32_t Encoder::getCount(void)
{
	    uint16_t counter; // 16 bit counter of timer
	    int32_t count32;  // 32 bit counter which accounts for timer overflows

	    counter =__HAL_TIM_GET_COUNTER(&Encoder_Handle);

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

	    count32 = overflows_*0x10000 + counter;
	    prev_counter_ = counter;

	    return count32;
}
float Encoder::getRevolutions(encoder_id_t encode)
{
	switch(encode)
	{
	case Azimuthal_Encoder:
		return(-1.0f * (getCount() / Pulses_Per_Revolution / Azimuthal_Gear_Ratio / Pinion_Spur_Gear_Ratio));
		break;
	case Vertical_Encoder:
		return(-1.0f * (getCount() / Pulses_Per_Revolution / Vertical_Gear_Ratio / Pinion_Spur_Gear_Ratio));
		break;
	case Claw_Encoder:
		return(-1.0f * (getCount() / Pulses_Per_Revolution / Claw_Gear_Ratio));
		break;
	default:
		return(0);
		break;
	}
}

float Encoder::getPosition(encoder_id_t encode)
{
	switch(encode)
	{
	case Azimuthal_Encoder:
		return((getRevolutions(Vertical_Encoder) / ThreadPitch) * Inches_to_Centimeters);
		break;
	case Vertical_Encoder:
		return(-1.0f * (getCount() / Pulses_Per_Revolution / Vertical_Gear_Ratio / Pinion_Spur_Gear_Ratio));
		break;
	case Claw_Encoder:
		return((getRevolutions(Claw_Encoder) / ThreadPitch) * Inches_to_Centimeters);
		break;
	default:
		return(0);
		break;
	}
}

void Encoder::setPosition(encoder_id_t encode, float pos)
{
	switch(encode)
	{
	case Vertical_Encoder:
		vertPos_ = pos;
		break;
	default:
		break;
	}
}

float Encoder::getDesiredPosition(encoder_id_t encode)
{
	switch(encode)
	{
	case Azimuthal_Encoder:
		return(0);
		break;
	case Vertical_Encoder:
		return(desiredVertPos_);
		break;
	case Claw_Encoder:
		return(0);
		break;
	}
}

void Encoder::setDesiredPosition(encoder_id_t encode, float pos)
{
	switch(encode)
	{
	case Vertical_Encoder:
		desiredVertPos_ = pos;
		break;
	}
}
bool Encoder::getDirection(void)
{
	/* Get the current direction */
	return(__HAL_TIM_IS_TIM_COUNTING_DOWN(&Encoder_Handle));
}

//*****************************************************************************
void Encoder::setCount(int32_t count32)
{
    if (count32 < 0)
    {
        overflows_ = count32 / ((int32_t)0x10000) - 1;
    }
    else
    {
        overflows_ = count32 / ((int32_t)0x10000);
    }

    uint16_t counter = (uint16_t)(count32 - overflows_*0x10000);
    prev_counter_ = counter;

    __HAL_TIM_SET_COUNTER(&Encoder_Handle, counter);
}

void Encoder::enableEncoder(encoder_id_t encode)
{
	switch(encode)
	{
	case Azimuthal_Encoder:
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET);  // Channel B
		HAL_GPIO_WritePin(GPIOI, GPIO_PIN_0, GPIO_PIN_SET);    // Channel A
		break;
	case Vertical_Encoder:
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);    // Channel B
		HAL_GPIO_WritePin(GPIOI, GPIO_PIN_0, GPIO_PIN_RESET);  // Channel A
		break;
	case Claw_Encoder:
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);    // Channel B
		HAL_GPIO_WritePin(GPIOI, GPIO_PIN_0, GPIO_PIN_SET);    // Channel A
		break;
	}
}
void Encoder::Error_Handler(void)
{
  /* Turn LED3 on */
  BSP_LED_On(LED1);
  while(1)
  {
  }
}
