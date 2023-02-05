/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <Drivers/Simulation/SI2C.h>
#include "device_tree.h"
#include "easy_log.h"
#include "ssd1306.h"
#include "gy30.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct __QueueData_t{
	uint8_t		send_task;
	uint8_t		received_task;
	char		*message;
	uint32_t	message_size;
	uint32_t	arg;			///< reserved (must be 0)
} QueueData_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

UART_HandleTypeDef huart2;

/* Definitions for ledTask */
osThreadId_t ledTaskHandle;
const osThreadAttr_t ledTask_attributes = {
  .name = "ledTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for GY30Task */
osThreadId_t GY30TaskHandle;
const osThreadAttr_t GY30Task_attributes = {
  .name = "GY30Task",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for OLEDTask */
osThreadId_t OLEDTaskHandle;
const osThreadAttr_t OLEDTask_attributes = {
  .name = "OLEDTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for RTCTask */
osThreadId_t RTCTaskHandle;
const osThreadAttr_t RTCTask_attributes = {
  .name = "RTCTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for gy30Queue */
osMessageQueueId_t gy30QueueHandle;
const osMessageQueueAttr_t gy30Queue_attributes = {
  .name = "gy30Queue"
};
/* Definitions for oledQueue */
osMessageQueueId_t oledQueueHandle;
const osMessageQueueAttr_t oledQueue_attributes = {
  .name = "oledQueue"
};
/* USER CODE BEGIN PV */
//uint8_t         txBuffer[1];
//uint8_t         rxBuffer[2];
SI2C_Handle      si2c0;
SI2C_Params      si2c0Params;
SI2C_Handle      si2c1;
SI2C_Params      si2c1Params;
//SI2C_Transaction si2cTransaction;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_RTC_Init(void);
void StartLedTask(void *argument);
void StartGY30Task(void *argument);
void StartOLEDTask(void *argument);
void StartRTCTask(void *argument);

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
  MX_USART2_UART_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  SI2C_init();
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of gy30Queue */
  gy30QueueHandle = osMessageQueueNew (8, sizeof(uint32_t), &gy30Queue_attributes);

  /* creation of oledQueue */
  oledQueueHandle = osMessageQueueNew (8, sizeof(QueueData_t), &oledQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of ledTask */
  ledTaskHandle = osThreadNew(StartLedTask, NULL, &ledTask_attributes);

  /* creation of GY30Task */
  GY30TaskHandle = osThreadNew(StartGY30Task, NULL, &GY30Task_attributes);

  /* creation of OLEDTask */
  OLEDTaskHandle = osThreadNew(StartOLEDTask, NULL, &OLEDTask_attributes);

  /* creation of RTCTask */
  RTCTaskHandle = osThreadNew(StartRTCTask, NULL, &RTCTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_SUNDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x53;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0x0;
  sAlarm.AlarmTime.Minutes = 0x0;
  sAlarm.AlarmTime.Seconds = 0x0;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_ALL;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 0x1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, SI2C1_SCL_Pin|SI2C1_SDA_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SI2C0_SCL_Pin|SI2C0_SDA_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SI2C1_SCL_Pin SI2C1_SDA_Pin */
  GPIO_InitStruct.Pin = SI2C1_SCL_Pin|SI2C1_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : SI2C0_SCL_Pin SI2C0_SDA_Pin */
  GPIO_InitStruct.Pin = SI2C0_SCL_Pin|SI2C0_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	BaseType_t xHigherPriorityTaskWoken;
	uint32_t ulStatusAlarm = 0x01;

	xHigherPriorityTaskWoken = pdFALSE;
	xTaskNotifyFromISR( RTCTaskHandle, ulStatusAlarm, eSetBits, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartLedTask */
/**
  * @brief  Function implementing the ledTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartLedTask */
void StartLedTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
	printf("Led task is run\r\n");

	for(;;)
	{
		printf("Toggle LED\r\n");
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		osDelay(3000);
	}
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartGY30Task */
/**
* @brief Function implementing the GY30Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartGY30Task */
void StartGY30Task(void *argument)
{
  /* USER CODE BEGIN StartGY30Task */
	uint32_t ulNotifiedValue;

	uint16_t lux;
	char lux_value[5] = "0000";
	unsigned char size;

	QueueData_t lux_queue = {
			.send_task		= TASK_GY30,
			.received_task	= TASK_OLED,
			.message		= lux_value,
			.message_size	= sizeof(lux_value),
			.arg			= 0
	};

	printf("GY-30 task is run\r\n");
	osDelay(200);

	/* GY-30 Software IIC init */
	SI2C_Params_init(&si2c0Params);
	si2c0Params.bitRate = SI2C_100kHz;
	si2c0Params.slaveAddress = GY30_I2C_ADDR;
	si2c0 = SI2C_open(DEVICE_SI2C0, &si2c0Params);
	if (si2c0 == NULL) {
	  printf("Error Initializing SI2C0\r\n");
	  while (1);
	}
	else {
	  printf("SI2C0 Initialized!\r\n");
	}

	GY30_Init(si2c0);
	osDelay(100);

  /* Infinite loop */
	for(;;)
	{
		xTaskNotifyWait( 0x00, 0xffffffff, &ulNotifiedValue, portMAX_DELAY );

		lux = GY30_Measurement(si2c0);
		printf("Illumination is: %d lux.\r\n", lux);

		int_to_ascii( (int)lux, lux_queue.message, &size, 4, ' ' );

		if( xQueueSend( oledQueueHandle, &lux_queue, ( TickType_t ) 0 ) != pdPASS )
		{
			printf("GY-30 Send, oled Queue is Full\r\n");
		}
	}
  /* USER CODE END StartGY30Task */
}

/* USER CODE BEGIN Header_StartOLEDTask */
/**
* @brief Function implementing the OLEDTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartOLEDTask */
void StartOLEDTask(void *argument)
{
  /* USER CODE BEGIN StartOLEDTask */
	BaseType_t xReturn = pdTRUE;
	QueueData_t oled_queue;

	uint8_t oled_counter = 0;

	printf("oled task is run\r\n");
	osDelay(500);

	/* OLED Software IIC init */
	SI2C_Params_init(&si2c1Params);
	si2c1Params.bitRate = SI2C_100kHz;
	si2c1Params.slaveAddress = SSD1306_I2C_ADDR;
	si2c1 = SI2C_open(DEVICE_SI2C1, &si2c1Params);
	if (si2c1 == NULL) {
	  printf("Error Initializing SI2C1\r\n");
	  while (1);
	}
	else {
	  printf("SI2C1 Initialized!\r\n");
	}

	OLED_Init(si2c1);
	SSD1306_Monochrome(si2c1, SSD1306_COLOR_BLACK);

  /* Infinite loop */
	for(;;)
	{
		xReturn = xQueueReceive( oledQueueHandle, &oled_queue, portMAX_DELAY);
		if(pdPASS == xReturn)
		{
			switch (oled_queue.send_task)
			{
			case TASK_RTC :
				SSD1306_SetCursor(8, 0);
				SSD1306_WriteString(oled_queue.message, Font_7x10, SSD1306_COLOR_WHITE);
				break;
			case TASK_GY30 :
				SSD1306_SetCursor(8, 36);
				SSD1306_WriteString(oled_queue.message, Font_7x10, SSD1306_COLOR_WHITE);
				SSD1306_SetCursor(43, 36);
				SSD1306_WriteString("lux", Font_7x10, SSD1306_COLOR_WHITE);
				break;
			default:
				break;
			}
		}

		if( (oled_counter >= 0) && (oled_counter < 10) )
		{
			SSD1306_ON(si2c1);
			SSD1306_UpdateScreen(si2c1);
		}
		if( (oled_counter >= 10) && (oled_counter < 30) )
		{
			SSD1306_OFF(si2c1);
		}
		if( oled_counter == 30 )
		{
			oled_counter = 0;
		}
		oled_counter++;
	}
  /* USER CODE END StartOLEDTask */
}

/* USER CODE BEGIN Header_StartRTCTask */
/**
* @brief Function implementing the RTCTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartRTCTask */
void StartRTCTask(void *argument)
{
  /* USER CODE BEGIN StartRTCTask */
	uint32_t ulNotifiedValue;
	uint32_t ulStatusRTC = 0x01;

	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};

	char rtc_time[17] = "2023/01/01-00:00";
	unsigned char size;

	QueueData_t rtc_queue = {
			.send_task		= TASK_RTC,
			.received_task	= TASK_OLED,
			.message		= rtc_time,
			.message_size	= sizeof(rtc_time),
			.arg			= 0
	};

	uint8_t rtc_counter = 1;

  /* Infinite loop */
	for(;;)
	{
		xTaskNotifyWait( 0x00, 0xffffffff, &ulNotifiedValue, portMAX_DELAY );

		if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
		{
			Error_Handler();
		}
		if (HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
		{
			Error_Handler();
		}
//		printf("%02d/%02d/%02d-%02d:%02d:%02d\r\n", 1970 + sDate.Year, sDate.Month, sDate.Date, sTime.Hours, sTime.Minutes, sTime.Seconds);

		int_to_ascii( (int)(1970 + sDate.Year), rtc_queue.message, &size, 4, '0' );
		int_to_ascii( (int)(sDate.Month), rtc_queue.message + 5, &size, 2, '0' );
		int_to_ascii( (int)(sDate.Date), rtc_queue.message + 8, &size, 2, '0' );
		int_to_ascii( (int)(sTime.Hours), rtc_queue.message + 11, &size, 2, '0' );
		int_to_ascii( (int)(sTime.Minutes), rtc_queue.message + 14, &size, 2, '0' );

		if( (rtc_counter == 0) || (rtc_counter == 60) )
		{
			printf("%s\r\n", rtc_time);
			if( xQueueSend( oledQueueHandle, &rtc_queue, ( TickType_t ) 0 ) != pdPASS )
			{
				printf("RTC Send, oled Queue is Full\r\n");
			}

			rtc_counter = 0;
		}

		if( (rtc_counter % 3) == 0 )
		{
			xTaskNotify( GY30TaskHandle, ulStatusRTC, eSetBits );
		}

		rtc_counter++;
	}
  /* USER CODE END StartRTCTask */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM22 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM22) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
