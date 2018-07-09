// Adapted from  STemWin/STemWin_HelloWorld/Src/main.c.  See LICENSE.

#include "main.h"
#include "stdlib.h"
#include "arm_math.h"

TIM_HandleTypeDef TimHandle;
TIM_HandleTypeDef MotorPWM;


#define kp 0.1f;
#define ki 0.000f;
#define kd 0.5f;

arm_pid_instance_f32 PID;

/*
 * Variables defining distance per count
 */
// counts-to-position
//float c2p_azimuthal = -float(Inches_to_Centimeters)/float(Pulses_Per_Revolution*
//                      Azimuthal_Gear_Ratio);// * Pinion_Spur_Gear_Ratio);//ThreadPitch* 360

float c2p_azimuthal = -0.00135858 / (360.0 / 357.0);

// Original in inch, divide the value in the parentheses by 2.54 to
// convert to cm
//float c2p_vertical = -1.0 / float(Pulses_Per_Revolution *
//		                          Vertical_Gear_Ratio *
//		                          Pinion_Spur_Gear_Ratio *
//								  ThreadPitch /
//								  2.54);
// c2p_vertical is about -6.22e-5 cm per count, or 16085 counts per cm

// use the calibrated value
float c2p_vertical = -6.3411541e-5;

float c2p_claw =  -float(Inches_to_Centimeters)/
		          float(Pulses_Per_Revolution*Claw_Gear_Ratio*ThreadPitch);

Motor motor_azimuthal(TIM10, c2p_azimuthal, 1, 5.0);

Motor motor_vertical(TIM11, c2p_vertical, 2, 0.5);

Motor motor_claw(TIM13, c2p_claw, 3, 0.5);

//Initialize pointer to select which motor is enabled, read encoders, ect.
//Motor *motor = &motor_vertical;

static void SystemClock_Config(void);
extern void MainTask(void);
static void EXTI15_10_IRQHandler_Config(void);
static void EXTI9_5_IRQHandler_Config(void);
static void EXTI0_IRQHandler_Config(void);

//----------------------------------------------------------------------------//
int main(void)
{
	/* Initializes Hardware Abstraction Layer libraries,
	 also implements callback functions in hal_msp.cpp */
	HAL_Init();

	/* Configure the system clock to 216 MHz */
	SystemClock_Config();

	// Used to refresh the touch screen and to convert the
	// STemWin screen info to HAL_GUI info
	BspInit();

	/* Configure LED1 */
	BSP_LED_Init(LED1);

	/* Configure Interrupt GPIO lines for limit switches*/
	EXTI15_10_IRQHandler_Config();
	EXTI9_5_IRQHandler_Config();
	EXTI0_IRQHandler_Config();

	/* Initializes CMSIS ARM_Math PID function*/
	PID.Kp = kp;
	PID.Ki = ki;
	PID.Kd = kd;
//	arm_pid_init_f32(&PID, 1);


	/***********************************************************/

	//This timer(TIM3) is used as an interupt to refresh the touchscreen
	/* Set TIMx instance */
	TimHandle.Instance = TIM3;

	/* Initialize TIM3 peripheral as follows:
	 + Period = 100 - 1
	 + Prescaler = 500 - 1
	 + TIM3 Clock is at 50 MHz
	 + ClockDivision = 0
	 + Counter Direction = Up
	 + TIM3 frequency = TIM3 clock / (Period * Prescaler)
	 + f = 50Mhz/100*500 = 1000Hz
	 */
	TimHandle.Init.Period = 99;
	TimHandle.Init.Prescaler = 499;
	TimHandle.Init.ClockDivision = 0;
	TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
	{
		while (1)
		{
		}
	}
	/*##-2- Start the TIM Base generation in interrupt mode ####################*/
	/* Start Channel1 */

	if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)
	{
		while (1)
		{
		}
	}
	/***Required initialization of pins before you can manipulate them***/
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

	/***********************************************************/

	/* Init the STemWin GUI Library */
	BSP_SDRAM_Init(); /* Initializes the SDRAM device */
	__HAL_RCC_CRC_CLK_ENABLE(); /* Enable the CRC Module */
	GUI_Init();
	GUI_SelectLayer(0);
	/* Activate the use of memory device feature */
	WM_SetCreateFlags(WM_CF_MEMDEV);

	while (1)
	{
		// Starts the MainTask() Function which is in an External .c file
		MainTask();
	}
}

//----------------------------------------------------------------------------//
// Period elapsed callback in non blocking mode
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	float threshold = 0.01;

	TouchUpdate();

	Motor* motor = NULL;
	if (motor_azimuthal.enabled())
		motor = &motor_azimuthal;
	else if (motor_vertical.enabled())
		motor = &motor_vertical;
	else if (motor_claw.enabled())
		motor = &motor_claw;

	// set duty for active motor
	if (motor)
	{
		// azimuthal rotates less than others so it needs a smaller duty
		int duty_command;
		if (motor -> get_id() == 2)
			// z motor
			duty_command = 100;
		else if (motor -> get_id() == 1)
			// azimuthal motor
			duty_command = 100;

		else if (motor -> get_id() == 3)
			// claw motor
			duty_command = 70;

		if (motor->getPosError() > threshold)
			// z motor goes up
			motor->setDuty(-duty_command);
		else if (motor->getPosError() < -threshold){
			motor->setDuty(duty_command);
			if (motor -> get_id() == 3 and motor -> get_claw_ls())
				motor -> set_claw_ls(false);
		}
		else
		{
			motor -> setDuty(0);
			// end of the cycle that apart 1cm from the top switch
			if (motor_vertical.get_top_ls())
				motor_vertical.set_top_ls(false);
		}
	}
}

//----------------------------------------------------------------------------//
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	/*
	 * The GPIN_PIN_10 port, which is used by the bottom limit switch, is
	 * broken. Disconnected.
	 * --Kevin
	 */
	BSP_LED_Off(LED1);
	// bottom switch limit (not used)
	if (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_10))
	{
		BSP_LED_On(LED1);
		motor_vertical.disable();
		motor_vertical.set_bot_ls(true);

		// Bound off after solving the wiring issue!!!
		// motor_vertical.setDesiredPosition(motor_vertical.getPosition());
		// motor_vertical.enable();
		// motor_vertical.decrease();
	}
	// top limit switch (fuel at top limit)
	else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0))
	{
		if (motor_vertical.enabled())
		{
			BSP_LED_On(LED1);
			motor_vertical.disable();
			motor_vertical.set_top_ls(true);

			// set origin at the top switch
			motor_vertical.set_zero();
//			motor = &motor_vertical;
			motor_vertical.enable();
			motor_vertical.setDesiredPosition(1.0);
		}
	}
	// claw switch limit. claw at fully open position
	else if (HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_9))
	{
		// effective when claw motor moves towards switch
		if (motor_claw.getDuty() < -10)
		{
			BSP_LED_On(LED1);
			motor_claw.disable();
			motor_claw.set_claw_ls(true);
			motor_claw.set_zero();
//			motor = &motor_claw;
			motor_claw.enable();
		}
	}
}

//----------------------------------------------------------------------------//
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
	if (ret != HAL_OK)
	{
		while (1)
		{
			;
		}
	}

	/* Activate the OverDrive to reach the 200 MHz Frequency */
	ret = HAL_PWREx_EnableOverDrive();
	if (ret != HAL_OK)
	{
		while (1)
		{
			;
		}
	}

	/* Select PLL as system clock source and configure the HCLK,
	 * PCLK1 and PCLK2 clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6);
	if (ret != HAL_OK)
	{
		while (1)
		{
			/* ... */
		}
	}
}

//----------------------------------------------------------------------------//
static void EXTI15_10_IRQHandler_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOF clock */
	__HAL_RCC_GPIOF_CLK_ENABLE();

	/* Configure PF.10 pin as input floating (Vertical bottom limit switch)*/
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pin = GPIO_PIN_10;
	//HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);

	/* Enable and set EXTI lines 15 to 10 Interrupt to the lowest priority */
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ (EXTI15_10_IRQn);
}

//----------------------------------------------------------------------------//
static void EXTI9_5_IRQHandler_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOF clock */
	__HAL_RCC_GPIOF_CLK_ENABLE();

	/* Configure PF.9 pin as input floating (claw limit switch)*/ 
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	GPIO_InitStructure.Pin = GPIO_PIN_9;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);

	/* Enable and set EXTI lines 9 to 5 Interrupt to the lowest priority */
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ (EXTI9_5_IRQn);
}

//----------------------------------------------------------------------------//
static void EXTI0_IRQHandler_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOF clock */
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/* Configure PF.9 pin as input floating (Vertical top limit switch)*/
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	GPIO_InitStructure.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Enable and set EXTI lines 9 to 5 Interrupt to the lowest priority */
	HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ (EXTI0_IRQn);
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
