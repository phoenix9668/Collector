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
#include "iwdg.h"
#include "usart.h"
#include "crc.h"
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
#define	_TAIL_SIZE                 2   // tail size
static uint8_t collectorIDBuffer[_COLLECTOR_ID_SIZE];
static uint8_t tailBuffer[2] = {0x0D, 0x0A};
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
osSemaphoreId rxBufferBinarySemHandle;
osSemaphoreId usartIdleBinarySemHandle;
osSemaphoreId dmaHTBinarySemHandle;
osSemaphoreId dmaTCBinarySemHandle;

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
	if (HAL_IWDG_Refresh(&hiwdg) != HAL_OK)
	{
		Error_Handler();
	}
	LED_COM_TOG();
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
	collectorIDBuffer[0] = (uint8_t)(0xFF & CollectorID>>24);
	collectorIDBuffer[1] = (uint8_t)(0xFF & CollectorID>>16);
	collectorIDBuffer[2] = (uint8_t)(0xFF & CollectorID>>8);
	collectorIDBuffer[3] = (uint8_t)(0xFF & CollectorID);
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of rxBufferBinarySem */
  osSemaphoreDef(rxBufferBinarySem);
  rxBufferBinarySemHandle = osSemaphoreCreate(osSemaphore(rxBufferBinarySem), 1);

  /* definition and creation of usartIdleBinarySem */
  osSemaphoreDef(usartIdleBinarySem);
  usartIdleBinarySemHandle = osSemaphoreCreate(osSemaphore(usartIdleBinarySem), 1);

  /* definition and creation of dmaHTBinarySem */
  osSemaphoreDef(dmaHTBinarySem);
  dmaHTBinarySemHandle = osSemaphoreCreate(osSemaphore(dmaHTBinarySem), 1);

  /* definition and creation of dmaTCBinarySem */
  osSemaphoreDef(dmaTCBinarySem);
  dmaTCBinarySemHandle = osSemaphoreCreate(osSemaphore(dmaTCBinarySem), 1);

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
  osThreadDef(rfidInputTask, StartRFIDInputTask, osPriorityHigh, 0, 128);
  rfidInputTaskHandle = osThreadCreate(osThread(rfidInputTask), NULL);

  /* definition and creation of commandTask */
  osThreadDef(commandTask, StartCommandTask, osPriorityBelowNormal, 0, 512);
  commandTaskHandle = osThreadCreate(osThread(commandTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	lte_usart_init();
	memset(&lte,0,sizeof(lte));
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
		LED_STA_TOG();
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
		osSemaphoreWait(rxBufferBinarySemHandle, osWaitForever);

		LED_RUN_ON();
		debug_printf("rxBuffer = %s\n",lte.rxBuffer);
		debug_printf("rxCounter = %d\n",lte.rxCounter);

		if(lte.rxBuffer[0] == 0xE5 && lte.rxBuffer[1] == 0x5E)
		{
			lte_usart_send_string("Header Right\r\n");
			FramCtrl(lte.rxBuffer);
		}
		else if(lte.rxBuffer[0] == (uint8_t)(0xFF & CollectorID>>24) && lte.rxBuffer[1] == (uint8_t)(0xFF & CollectorID>>16) && lte.rxBuffer[2] == (uint8_t)(0xFF & CollectorID>>8) && lte.rxBuffer[3] == (uint8_t)(0xFF & CollectorID))
		{
			lte_usart_send_string("CollectorID Right\r\n");
			FunctionCtrl(lte.rxBuffer);
		}
		memset(&lte,0,sizeof(lte));
    LED_RUN_OFF();
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
	if(command[4] == 0xA0){
		__set_FAULTMASK(1);
		NVIC_SystemReset();
	}
	else if(command[4] == 0xA8){
		for(uint8_t i=0; i<3; i++)
		{	timeBuffer[i] = command[9+i];}
		for(uint8_t i=0; i<4; i++)
		{	dateBuffer[i] = command[5+i];}

		SetRTC(timeBuffer, dateBuffer);
		SendToCloud(command[4], _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + _UTC_TIME_SIZE + _TAIL_SIZE);
	}
	else if(command[4] == 0xA9){
		SendToCloud(command[4], _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + _UTC_TIME_SIZE + _TAIL_SIZE);
	}
	else
		lte_usart_send_string("FunctionID Unused\r\n");
}

/*===========================================================================
* void SendToCloud(uint8_t functionID, uint8_t length) => send data to cloud* 
============================================================================*/
void SendToCloud(uint8_t functionID, uint8_t length)
{
	uint8_t timeBuffer[3];
	uint8_t dateBuffer[4];
	uint8_t crcBuffer[4];
	uint8_t sendFunctionID;

	if(functionID == 0x30)
	{
		GetRTC(timeBuffer, dateBuffer);
		strcatArray(sendToCloudBuffer, collectorIDBuffer, 0, 0);
		
		if (cc1101.recvBuffer[38] == 0xD1 && cc1101.recvBuffer[39] == 0xD1){ 
			sendFunctionID = 0xB1;
			length = length - 2;
		}
		else
		{ sendFunctionID = 0xB0;}

		strcatArray(sendToCloudBuffer, &sendFunctionID, _COLLECTOR_ID_SIZE, 0);
		strcatArray(sendToCloudBuffer, cc1101.recvBuffer, _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE, length - sizeof(cc1101.crcValue));
    strcatArray(sendToCloudBuffer, &cc1101.rssi, _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + length - sizeof(cc1101.crcValue), 0);
	  strcatArray(sendToCloudBuffer, dateBuffer, _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + length - sizeof(cc1101.crcValue) + sizeof(cc1101.rssi), 0);
    strcatArray(sendToCloudBuffer, timeBuffer, _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + length - sizeof(cc1101.crcValue) + sizeof(cc1101.rssi) + sizeof(dateBuffer), 0);
		
		cc1101.crcValue = CRC32_Calculate(sendToCloudBuffer, (uint32_t)(_COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + length - sizeof(cc1101.crcValue) + sizeof(cc1101.rssi) + _UTC_TIME_SIZE), CRC_INPUTDATA_FORMAT_BYTES);
		crcBuffer[0] = (uint8_t)(0xFF & cc1101.crcValue>>24);
		crcBuffer[1] = (uint8_t)(0xFF & cc1101.crcValue>>16);
		crcBuffer[2] = (uint8_t)(0xFF & cc1101.crcValue>>8);
		crcBuffer[3] = (uint8_t)(0xFF & cc1101.crcValue);
		strcatArray(sendToCloudBuffer, crcBuffer, _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + length - sizeof(cc1101.crcValue) + sizeof(cc1101.rssi) + _UTC_TIME_SIZE, 0);
		strcatArray(sendToCloudBuffer, tailBuffer, _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + length + sizeof(cc1101.rssi) + _UTC_TIME_SIZE, 0);
		
    lte_usart_send_data((uint8_t*)sendToCloudBuffer,_COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + length + sizeof(cc1101.rssi) + _UTC_TIME_SIZE + _TAIL_SIZE);
		
		for(uint8_t i=0; i<_COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + length + sizeof(cc1101.rssi) + _UTC_TIME_SIZE; i++){
			debug_printf("%02x ",sendToCloudBuffer[i]);}
		debug_printf("\r\n");
	}
	else if(functionID == 0xA8 || functionID == 0xA9)
	{
		GetRTC(timeBuffer, dateBuffer);
		strcatArray(sendToCloudBuffer, collectorIDBuffer, 0, 0);
		sendFunctionID = 0xB0 + (0x0F & functionID);
		strcatArray(sendToCloudBuffer, &sendFunctionID, _COLLECTOR_ID_SIZE, 0);
	  strcatArray(sendToCloudBuffer, dateBuffer, _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE, 0);
    strcatArray(sendToCloudBuffer, timeBuffer, _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + sizeof(dateBuffer), 0);
		strcatArray(sendToCloudBuffer, tailBuffer, _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + _UTC_TIME_SIZE, 0);	

    lte_usart_send_data(sendToCloudBuffer,length);
		
		for(uint8_t i=0; i<_COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + _UTC_TIME_SIZE; i++){
			debug_printf("%02x ",sendToCloudBuffer[i]);}
		debug_printf("\r\n");
	}
	else if(functionID == 0x01)
	{	debug_printf("receive error or address filtering fail\r\n");}
	else if(functionID == 0x02)
	{	debug_printf("rfid receive crc error\r\n");}

}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
