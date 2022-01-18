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
#include "rtc.h"
#include "usart.h"
#include "cc1101.h"
#include "fram.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define	_SEND_TO_CLOUD_BUFFERSIZE  256 // tx buffer size
#define	_COLLECTOR_ID_SIZE         4   // collectorID size
#define	_FUNCTION_ID_SIZE          1   // functionID size
#define	_UTC_TIME_SIZE             7   // UTC time size
static uint8_t sendToCloudBuffer[_SEND_TO_CLOUD_BUFFERSIZE];
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
void FunctionCtrl(uint8_t *command);
void SendToCloud(uint8_t functionID, uint8_t length);
/* USER CODE END FunctionPrototypes */

void StartUsartRxDmaTask(void const * argument);
void StartRFIDInputTask(void const * argument);
void StartCommandTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void vApplicationIdleHook(void);

/* USER CODE BEGIN 2 */
__weak void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
  SendToCloud(CC1101RecvHandler(),cc1101.length);
}
/* USER CODE END 2 */

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
		LED_COM_TOG();
		RFIDInitial(0x00, 0x1234, RX_MODE);
    osDelay(600000);
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
				debug_printf("Header Right\n");
				FramCtrl(lte.rxBuffer);
			}
			else if(lte.rxBuffer[0] == (uint8_t)(0xFF & CollectorID>>24) && lte.rxBuffer[1] == (uint8_t)(0xFF & CollectorID>>16) && lte.rxBuffer[2] == (uint8_t)(0xFF & CollectorID>>8) && lte.rxBuffer[3] == (uint8_t)(0xFF & CollectorID))
			{
				debug_printf("CollectorID Right\n");
				FunctionCtrl(lte.rxBuffer);
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

/*===========================================================================
* FunctionCtrl(uint8_t *command) => handle PC command                    	* 
============================================================================*/
void FunctionCtrl(uint8_t *command)
{
	uint8_t timeBuffer[3];
	uint8_t dateBuffer[4];

	/*A8:configure time information*/
	/*A9:read time information*/
	if(command[4] == 0xA8)
	{
		for(uint8_t i=0; i<3; i++)
		{	timeBuffer[i] = command[9+i];}
		for(uint8_t i=0; i<4; i++)
		{	dateBuffer[i] = command[5+i];}

		SetRTC(timeBuffer, dateBuffer);
		SendToCloud(command[4], _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + _UTC_TIME_SIZE);
	}
	else if(command[4] == 0xA9)
	{
		SendToCloud(command[4], _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + _UTC_TIME_SIZE);
	}
	else
		debug_printf("FunctionID Unused\n");
}

/*===========================================================================
* void SendToCloud(uint8_t functionID, uint8_t length) => send data to cloud* 
============================================================================*/
void SendToCloud(uint8_t functionID, uint8_t length)
{
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};

	if(functionID == 0x30)
	{
		GetRTC(&sTime, &sDate);
		sendToCloudBuffer[0] = (uint8_t)(0xFF & CollectorID>>24);
    sendToCloudBuffer[1] = (uint8_t)(0xFF & CollectorID>>16);
    sendToCloudBuffer[2] = (uint8_t)(0xFF & CollectorID>>8);
    sendToCloudBuffer[3] = (uint8_t)(0xFF & CollectorID);
		sendToCloudBuffer[4] = 0xB0;
		for(uint8_t i=0;i<length;i++)
		{
			sendToCloudBuffer[5+i] = cc1101.recvBuffer[i];
		}
		sendToCloudBuffer[length+5] = cc1101.rssi;
		sendToCloudBuffer[length+6] = sDate.Year;
		sendToCloudBuffer[length+7] = sDate.Month;
		sendToCloudBuffer[length+8] = sDate.Date;
		sendToCloudBuffer[length+9] = sDate.WeekDay;
		sendToCloudBuffer[length+10] = sTime.Hours;
		sendToCloudBuffer[length+11] = sTime.Minutes;
		sendToCloudBuffer[length+12] = sTime.Seconds;
		
    lte_usart_send_data((uint8_t*)sendToCloudBuffer,_COLLECTOR_ID_SIZE + length + 1 + _FUNCTION_ID_SIZE + _UTC_TIME_SIZE);
		
		for(uint8_t i=0; i<_COLLECTOR_ID_SIZE + length + 1 + _FUNCTION_ID_SIZE + _UTC_TIME_SIZE; i++){
			debug_printf("%x ",sendToCloudBuffer[i]);}
		debug_printf("\n");
	}
	else if(functionID == 0xA8 || functionID == 0xA9)
	{
		GetRTC(&sTime, &sDate);
		sendToCloudBuffer[0] = (uint8_t)(0xFF & CollectorID>>24);
    sendToCloudBuffer[1] = (uint8_t)(0xFF & CollectorID>>16);
    sendToCloudBuffer[2] = (uint8_t)(0xFF & CollectorID>>8);
    sendToCloudBuffer[3] = (uint8_t)(0xFF & CollectorID);		
		sendToCloudBuffer[4] = 0xB0 + (0x0F & functionID);
		sendToCloudBuffer[5] = sDate.Year;
		sendToCloudBuffer[6] = sDate.Month;
		sendToCloudBuffer[7] = sDate.Date;
		sendToCloudBuffer[8] = sDate.WeekDay;
		sendToCloudBuffer[9] = sTime.Hours;
		sendToCloudBuffer[10] = sTime.Minutes;
		sendToCloudBuffer[11] = sTime.Seconds;

    lte_usart_send_data(sendToCloudBuffer,length);
	}
	else if(functionID == 0x01)
	{	debug_printf("receive error or address filtering fail\r\n");}
	else if(functionID == 0x02)
	{	debug_printf("rfid receive crc error\r\n");}

}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
