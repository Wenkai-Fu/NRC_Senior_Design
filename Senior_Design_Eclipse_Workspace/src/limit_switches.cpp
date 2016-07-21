/*
 * limit_switches.cpp
 *
 *  Created on: Jul 21, 2016
 *      Author: smith
 */

#include "limit_switches.h"
#include "stm32f7xx_hal.h"

Limit::Limit(void)
	:switch_id_ (Top)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	// Enable GPIO Ports
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

	GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

bool Limit::getState(limit_id_t id)
{
	switch (id)
	{
	case Top:
		return HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
		break;
	case Bottom:
		return HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_10);
		break;
	case Claw:
		return HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_9);
		break;
	default:
		return 0;
	}
}
