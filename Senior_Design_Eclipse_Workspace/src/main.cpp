/**
  ******************************************************************************
  * @file    STemWin/STemWin_HelloWorld/Src/main.c
  * @author  MCD Application Team
  * @version V1.0.2
  * @date    18-November-2015
  * @brief   This file provides main program functions
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

uint8_t GUI_Initialized = 0;
TIM_HandleTypeDef TimHandle;
TIM_HandleTypeDef MotorPWM;

//Motor AzimuthalMotor(Azimuthal_Motor);



uint16_t dir;

float AzimuthalRevolutions, VerticalRevolutions, ClawRevolutions;
float AzimuthalSpeed, VerticalSpeed, ClawSpeed;
float AzimuthalDistance, VerticalDistance, ClawDistance;
int32_t AzimuthalCount, VerticalCount, ClawCount;


TIM_OC_InitTypeDef sConfig;

Motor AzimuthalMotor(Azimuthal_Motor);
Motor VerticalMotor(Vertical_Motor);
Motor ClawMotor(Claw_Motor);
Encoder encoder;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);



extern void MainTask(void);


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Enable the CPU Cache */
  // CPU_CACHE_Enable();

  /* STM32F7xx HAL library initialization:
       - Configure the Flash ART accelerator on ITCM interface
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
  HAL_Init();

  /* Configure the system clock to 200 MHz */
  SystemClock_Config();

  // Used to refresh the touch screen and to convert the STemWin screen info to HAL_GUI info
  BspInit();

  /* Configure LED1 */
  BSP_LED_Init(LED1);



  /***********************************************************/

  //This timer(TIM3) is used as an interupt to refresh the touchscreen




  /* Set TIMx instance */
  TimHandle.Instance = TIM3;

  /* Initialize TIM3 peripheral as follows:

       + Period = 500 - 1
       + Prescaler = ((SystemCoreClock/2)/10000) - 1
       + TIM3 Clock is at 50 MHz
       + ClockDivision = 0
       + Counter direction = Up
  */
  TimHandle.Init.Period = 99;
  TimHandle.Init.Prescaler = 499;
  TimHandle.Init.ClockDivision = 0;
  TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  if(HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
  {
    while(1)
    {
    }
  }
	/*##-2- Start the TIM Base generation in interrupt mode ####################*/
	/* Start Channel1 */

	if(HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)
	{
	while(1)
		{
		}
	}


		GPIO_InitTypeDef   GPIO_InitStruct;

		// Enable GPIO Ports
		__HAL_RCC_GPIOI_CLK_ENABLE();
		__HAL_RCC_GPIOG_CLK_ENABLE();

		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;


		GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_3;
		HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_7;
		HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);



  /***********************************************************/

  /* Init the STemWin GUI Library */
  BSP_SDRAM_Init(); /* Initializes the SDRAM device */
  __HAL_RCC_CRC_CLK_ENABLE(); /* Enable the CRC Module */
  GUI_Init();

  GUI_DispStringAt("Starting...", 0, 0);

  GUI_Clear();

  GUI_Initialized = 1;

  /* Activate the use of memory device feature */
  WM_SetCreateFlags(WM_CF_MEMDEV);




	while(1)
	{
		// Starts the MainTask() Function which is in an External .c file
		MainTask();

	}


}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim: TIM handle
  * @retval None
  */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    const float DT = 0.001f;
    float VerticalSpeedCommand = 0.5f;
    DerivativeFilter VerticalDerivative(0.001f, 10.0f, 0.707f);
    PidController VerticalSpeedPid(20.0, 20.0, 0, -1, 1, -4, 4);
	TouchUpdate();

	if (EncoderEnable[0] == true)
	{
	AzimuthalCount = encoder.read();
	dir = encoder.direction();
	AzimuthalRevolutions = -1.0f * (AzimuthalCount / Pulses_Per_Revolution / Azimuthal_Gear_Ratio);

	HAL_GPIO_WritePin(GPIOG,GPIO_PIN_7,GPIO_PIN_RESET);  // Channel B
	HAL_GPIO_WritePin(GPIOI,GPIO_PIN_0,GPIO_PIN_SET);    // Channel A

		if (AzimuthalRevolutions > 0.5f) AzimuthalMotor.dutyCycle(0);
		else AzimuthalMotor.dutyCycle(100);
	}


	if (EncoderEnable[1] == true)
	{
	float VerticalDeltaDistance = 0.0f;
	float DeltaRevolution;
	int32_t DeltaCount;

	DeltaCount = encoder.read() - VerticalCount;
	VerticalCount = encoder.read();
	dir = encoder.direction();

	VerticalRevolutions = -1.0f * (VerticalCount / Pulses_Per_Revolution / Vertical_Gear_Ratio);
	DeltaRevolution = -1.0f * (DeltaCount / Pulses_Per_Revolution / Vertical_Gear_Ratio);

	VerticalDistance = VerticalRevolutions / ThreadPitch;
	VerticalDeltaDistance = DeltaRevolution / ThreadPitch;

//	VerticalSpeed = VerticalDeltaDistance / 0.001f;
    VerticalSpeed = VerticalDerivative.calculate(VerticalDeltaDistance);
    VerticalSpeed = VerticalSpeed * 1000.0f;
//    float left_duty_command = VerticalSpeedPid.calculate(VerticalSpeedCommand - VerticalSpeed, DT);

	HAL_GPIO_WritePin(GPIOG,GPIO_PIN_7,GPIO_PIN_SET);    // Channel B
	HAL_GPIO_WritePin(GPIOI,GPIO_PIN_0,GPIO_PIN_RESET);  // Channel A

		if (VerticalDistance > 50.0f)
		{
		  VerticalMotor.dutyCycle(0);
		}
	}

	if (EncoderEnable[2] == true)
	{

	ClawCount = encoder.read();

	dir = encoder.direction();

	ClawRevolutions = -1.0f * (ClawCount / Pulses_Per_Revolution / Claw_Gear_Ratio);

	ClawDistance = ClawRevolutions / ThreadPitch;

	HAL_GPIO_WritePin(GPIOG,GPIO_PIN_7,GPIO_PIN_SET);    // Channel B
	HAL_GPIO_WritePin(GPIOI,GPIO_PIN_0,GPIO_PIN_SET);    // Channel A
	}


}




/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 200000000
  *            HCLK(Hz)                       = 200000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 400
  *            PLL_P                          = 2
  *            PLL_Q                          = 8
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 6
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 400;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;

  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }

  /* Activate the OverDrive to reach the 200 MHz Frequency */
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
}




#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/