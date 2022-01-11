/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gpio.h"
#include "usart.h"
#include "cc1101.h"
#include "fram.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId usartRxDmaTaskHandle;
osThreadId rfidInputTaskHandle;
osThreadId commandTaskHandle;
osMessageQId usartRxQueueHandle;
osSemaphoreId commandBinarySemHandle;
osSemaphoreId rfidInputBinarySemHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartUsartRxDmaTask(void const * argument);
void StartRFIDInputTask(void const * argument);
void StartCommandTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of commandBinarySem */
  osSemaphoreDef(commandBinarySem);
  commandBinarySemHandle = osSemaphoreCreate(osSemaphore(commandBinarySem), 1);

  /* definition and creation of rfidInputBinarySem */
  osSemaphoreDef(rfidInputBinarySem);
  rfidInputBinarySemHandle = osSemaphoreCreate(osSemaphore(rfidInputBinarySem), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of usartRxQueue */
  osMessageQDef(usartRxQueue, 16, uint32_t);
  usartRxQueueHandle = osMessageCreate(osMessageQ(usartRxQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of usartRxDmaTask */
  osThreadDef(usartRxDmaTask, StartUsartRxDmaTask, osPriorityNormal, 0, 512);
  usartRxDmaTaskHandle = osThreadCreate(osThread(usartRxDmaTask), NULL);

  /* definition and creation of rfidInputTask */
  osThreadDef(rfidInputTask, StartRFIDInputTask, osPriorityHigh, 0, 512);
  rfidInputTaskHandle = osThreadCreate(osThread(rfidInputTask), NULL);

  /* definition and creation of commandTask */
  osThreadDef(commandTask, StartCommandTask, osPriorityBelowNormal, 0, 128);
  commandTaskHandle = osThreadCreate(osThread(commandTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartUsartRxDmaTask */
/**
  * @brief  Function implementing the usartRxDmaTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartUsartRxDmaTask */
void StartUsartRxDmaTask(void const * argument)
{
  /* USER CODE BEGIN StartUsartRxDmaTask */
  /* Infinite loop */
  for(;;)
  {
		/* Block thread and wait for event to process USART data */
		osMessageGet(usartRxQueueHandle, osWaitForever);

		/* Simply call processing function */
		lte_usart_rx_check();
		
  }
  /* USER CODE END StartUsartRxDmaTask */
}

/* USER CODE BEGIN Header_StartRFIDInputTask */
/**
* @brief Function implementing the rfidInputTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartRFIDInputTask */
void StartRFIDInputTask(void const * argument)
{
  /* USER CODE BEGIN StartRFIDInputTask */
  /* Infinite loop */
  for(;;)
  {
		RF_Receive();
    osDelay(1);
  }
  /* USER CODE END StartRFIDInputTask */
}

/* USER CODE BEGIN Header_StartCommandTask */
/**
* @brief Function implementing the commandTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCommandTask */
void StartCommandTask(void const * argument)
{
  /* USER CODE BEGIN StartCommandTask */
  /* Infinite loop */
  for(;;)
  {
		if(lte.rxIndex == true)
		{
			LED_STA_ON();
		  debug_printf("rxBuffer = %s\n",lte.rxBuffer);
			debug_printf("rxCounter = %d\n",lte.rxCounter);

			if(lte.rxBuffer[0] == 0xE5 && lte.rxBuffer[1] == 0x5E)
			{
				debug_printf("fram transfer\n");
				Fram_Ctrl(lte.rxBuffer);
			}
			else if(lte.rxBuffer[0] == (uint8_t)(0xFF & MainBoardID>>8) && lte.rxBuffer[1] == (uint8_t)(0xFF & MainBoardID))
			{
				debug_printf("start transfer\n");
			}
			LED_STA_OFF();
			memset(&lte,0,sizeof(lte));
		}
    osDelay(1);
  }
  /* USER CODE END StartCommandTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
