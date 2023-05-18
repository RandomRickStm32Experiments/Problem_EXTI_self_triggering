/* USER CODE BEGIN Header */
/**
 *
 * WORKING VERY WELL. MAGIC WORTH KEEPING.
 *
 * PA0 is configured as a common-drain but with EXTI - not a combo available from CubeMX.
 *     magic sauce is added to MX_GPIO_Init() to turn just common-drain into EXTI as well.
 *
 * PA0 has an external switch to GND, simulating another common drain device connected.
 * This is fine. but this code also asserts PA0 low, and we don't want that creating an
 * EXTI interrupt - we only want interrupts on the same line from outside.
 *
 * So:
 * 1. EXTI ISR will react to external interrupt and start htim17 which begins looking at PA0
 *    to transfer data between devices.
 * 2. We don't want this subsequent activity causing interrupts: beyond the start bit,
 *    we use a time-loop to read the port.
 *
 * 3. Therefore, we turn off the EXTI IRQs when the start bit is detected, and
 *    turn them back on again at the end of the data packet.
 *
 * 4. But the turn-on causes a fresh EXTI IRQ, even though there's not -ve edge
 *     on PA0!
 *
 * 5. Look at HAL_TIM_PeriodElapsedCallBack() for the solution - the while loop just
 *    before NVIC_EnableIRQ()
 *
 * 6. SOlution found here: https://community.st.com/s/question/0D50X00009XkgqeSAB/clear-pending-interrupts
 *
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim17;

/* USER CODE BEGIN PV */
uint8_t htim17_tick = 0;
uint8_t halfBitCounter = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM17_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM17_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
  while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0)
  {
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {

  }
  LL_SetSystemCoreClock(8000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM17 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM17_Init(void)
{

  /* USER CODE BEGIN TIM17_Init 0 */

  /* USER CODE END TIM17_Init 0 */

  /* USER CODE BEGIN TIM17_Init 1 */

  /* USER CODE END TIM17_Init 1 */
  htim17.Instance = TIM17;
  htim17.Init.Prescaler = 8000-1;
  htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim17.Init.Period = 500-1;
  htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim17.Init.RepetitionCounter = 0;
  htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM17_Init 2 */

  /* USER CODE END TIM17_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
  LL_EXTI_InitTypeDef EXTI_InitStruct = {0};

   LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE0);


   EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_0;
   EXTI_InitStruct.LineCommand = ENABLE;
   EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
   EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
   LL_EXTI_Init(&EXTI_InitStruct);


/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

  /**/
  LL_GPIO_SetOutputPin(DualPin_GPIO_Port, DualPin_Pin);

  /**/
  LL_GPIO_ResetOutputPin(LED_GPIO_Port, LED_Pin);

  /**/
  GPIO_InitStruct.Pin = DualPin_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(DualPin_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */


  /* EXTI interrupt init*/
   NVIC_SetPriority(EXTI0_1_IRQn, 0);
   NVIC_EnableIRQ(EXTI0_1_IRQn);


/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */




void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	switch (halfBitCounter)
	{
	case 2:
	case 4:
		LL_GPIO_TogglePin(DualPin_GPIO_Port, DualPin_Pin);
		LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		break;

	case 8:
		HAL_TIM_Base_Stop_IT(&htim17);

		while (HAL_NVIC_GetPendingIRQ(EXTI0_1_IRQn))	// or	NVIC_GetPendingIRQ(EXTI0_1_IRQn)
		{
			__HAL_GPIO_EXTI_CLEAR_IT(LL_EXTI_LINE_0); 	// or	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_0);
			HAL_NVIC_ClearPendingIRQ(EXTI0_1_IRQn);		// or	NVIC_ClearPendingIRQ (EXTI0_1_IRQn);
		}

		NVIC_EnableIRQ(EXTI0_1_IRQn);
		break;
	default:
		break;
	}

	halfBitCounter++;
}






void EXTI0_1_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_1_IRQn 0 */

  /* USER CODE END EXTI0_1_IRQn 0 */
  if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_0) != RESET)
  {
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_0);
    /* USER CODE BEGIN LL_EXTI_LINE_0 */

    NVIC_DisableIRQ(EXTI0_1_IRQn);

    halfBitCounter = 0;

    if (htim17.State == HAL_TIM_STATE_READY)
    {
    	HAL_TIM_Base_Start_IT(&htim17);
    }


    /* USER CODE END LL_EXTI_LINE_0 */
  }
  /* USER CODE BEGIN EXTI0_1_IRQn 1 */

  /* USER CODE END EXTI0_1_IRQn 1 */
}











/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
