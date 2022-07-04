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
#include "sgm58031.h"
#include "lptim.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define EEPROM_DATA_LENGTH	16	// buffer length
#define	_SEND_TO_CLOUD_BUFFERSIZE  256 // tx buffer size
#define	_ADC_VALUE_BUFFERSIZE      9   // ADC value buffer size
#define	_COLLECTOR_ID_SIZE         4   // collectorID size
#define	_FUNCTION_ID_SIZE          1   // functionID size
#define	_UTC_TIME_SIZE             7   // UTC time size
#define	_TAIL_SIZE                 2   // tail size
static uint8_t collectorIDBuffer[_COLLECTOR_ID_SIZE];
static uint8_t tailBuffer[2] = {0x0D, 0x0A};
static uint8_t sendToCloudBuffer[_SEND_TO_CLOUD_BUFFERSIZE];
static uint8_t adcValueBuffer[_ADC_VALUE_BUFFERSIZE];
static uint16_t TwoHoursCnt = 0;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId usartRxDmaTaskHandle;
osThreadId iicConvertTaskHandle;
osThreadId usartRxCmdTaskHandle;
osMessageQId usartRxQueueHandle;
osSemaphoreId rxBufferBinarySemHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void EepromCtrl(uint8_t *command);
void FunctionCtrl(uint8_t *command);
void SendToCloud(uint8_t functionID, uint8_t length);
/* USER CODE END FunctionPrototypes */

void StartUsartRxDmaTask(void const * argument);
void StartIICConvertTask(void const * argument);
void StartUsartRxCmdTask(void const * argument);

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
    SendToCloud(CC1101RecvHandler(), cc1101.length);

    if (HAL_IWDG_Refresh(&hiwdg) != HAL_OK)
    {
        Error_Handler();
    }
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
    collectorIDBuffer[0] = (uint8_t)(0xFF & CollectorID >> 24);
    collectorIDBuffer[1] = (uint8_t)(0xFF & CollectorID >> 16);
    collectorIDBuffer[2] = (uint8_t)(0xFF & CollectorID >> 8);
    collectorIDBuffer[3] = (uint8_t)(0xFF & CollectorID);
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of rxBufferBinarySem */
  osSemaphoreDef(rxBufferBinarySem);
  rxBufferBinarySemHandle = osSemaphoreCreate(osSemaphore(rxBufferBinarySem), 1);

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
  osThreadDef(usartRxDmaTask, StartUsartRxDmaTask, osPriorityHigh, 0, 128);
  usartRxDmaTaskHandle = osThreadCreate(osThread(usartRxDmaTask), NULL);

  /* definition and creation of iicConvertTask */
  osThreadDef(iicConvertTask, StartIICConvertTask, osPriorityLow, 0, 128);
  iicConvertTaskHandle = osThreadCreate(osThread(iicConvertTask), NULL);

  /* definition and creation of usartRxCmdTask */
  osThreadDef(usartRxCmdTask, StartUsartRxCmdTask, osPriorityNormal, 0, 128);
  usartRxCmdTaskHandle = osThreadCreate(osThread(usartRxCmdTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    // follow code must be here!!!!!!
    ec600x_usart_init();
    memset(&lte, 0, sizeof(lte));
    Enable_LPUART1();
    ShowMessage();

    #if (_SGM58031 == 1)

    if (SGM58031_Init(SGM58031_ADDR) != SGM58031_OK)
    {
        /* Initialization false */
        printf("#----sgm58031 initial false~!----#\n");
    }

    #endif

    HAL_LPTIM_Counter_Start_IT(&hlptim1, 0x3FF);
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
        ec600x_usart_rx_check();
    }

  /* USER CODE END StartUsartRxDmaTask */
}

/* USER CODE BEGIN Header_StartIICConvertTask */
/**
* @brief Function implementing the iicConvertTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartIICConvertTask */
void StartIICConvertTask(void const * argument)
{
  /* USER CODE BEGIN StartIICConvertTask */
    uint16_t adcValue;

    /* Infinite loop */
    for(;;)
    {
        if(secITStatus == SET)
        {
            LED1_TOG();
            TwoHoursCnt++;

            if (TwoHoursCnt >= 1800)
            {
                CC1101_POWER_DOWN();
                osDelay(100);
                CC1101_POWER_ON();
                RFIDInitial(0x00, 0x1234, RX_MODE);
                ModuleLtePowerOn();
                TwoHoursCnt = 0;
            }

            #if (_SGM58031 == 1)

            /*##-1- read the OS of Configuration Register #####################################*/
            if (SGM58031_ReadReg(SGM58031_ADDR, SGM58031_CONF_REG) >> 15 == 1)
            {
                adcValue = SGM58031_ReadReg(SGM58031_ADDR, SGM58031_CONVERSION_REG);
                debug_printf("adcValue = %.3f\n", ((float)adcValue / 32768) * 4.096);

                strcatArray(adcValueBuffer, collectorIDBuffer, 0, 0);
                adcValueBuffer[4] = 0xB2;

                if(TwoHoursCnt % 2 == 1)
                {
                    adcValueBuffer[5] = (uint8_t)(adcValue >> 8);
                    adcValueBuffer[6] = (uint8_t)adcValue;
                }
                else
                {
                    adcValueBuffer[7] = (uint8_t)(adcValue >> 8);
                    adcValueBuffer[8] = (uint8_t)adcValue;
                }

//				for(uint8_t i=0; i<_ADC_VALUE_BUFFERSIZE; i++){
//			    printf("%02x ",adcValueBuffer[i]);}
//		    printf("\r\n");

//				lte_lpuart_send_string((const char*)adcValueBuffer);
            }

            /*##-2- Initialise the SGM58031 peripheral ####################################*/
            if (TwoHoursCnt % 2 == 1)
            {
                SGM58031_ConfigReg(Start_Single_Conversion, AINP_AIN0_AND_AINN_AIN1, FS_4_096V, Single_Shot_Mode, DR_800Hz_960Hz, Traditional_Comparator, Active_Low, Non_Latching, Disable_Comparator, DR_SEL0);
            }
            else
            {
                SGM58031_ConfigReg(Start_Single_Conversion, AINP_AIN2_AND_AINN_AIN3, FS_4_096V, Single_Shot_Mode, DR_800Hz_960Hz, Traditional_Comparator, Active_Low, Non_Latching, Disable_Comparator, DR_SEL0);
            }

            /*##-3- Set the Configuration Register #####################################*/
            if (SGM58031_WriteReg(SGM58031_ADDR, SGM58031_CONF_REG, SGM58031_InitStructure.Config_Register_Value) != SGM58031_OK)
            {
                Error_Handler();
            }

            #endif

            secITStatus = RESET;
        }

        osDelay(1);
    }

  /* USER CODE END StartIICConvertTask */
}

/* USER CODE BEGIN Header_StartUsartRxCmdTask */
/**
* @brief Function implementing the usartRxCmdTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUsartRxCmdTask */
void StartUsartRxCmdTask(void const * argument)
{
  /* USER CODE BEGIN StartUsartRxCmdTask */
    /* Infinite loop */
    for(;;)
    {
        osSemaphoreWait(rxBufferBinarySemHandle, osWaitForever);

        LED2_ON();
        lwrb_read(&ec600x_usart_rx_rb, lte.rxBuffer, lte.rxCounter);

        debug_printf("rxBuffer =");

        for(uint8_t i = 0; i < lte.rxCounter; i++)
        {
            debug_printf(" %x", lte.rxBuffer[i]);
        }

        debug_printf("\nrxCounter = %d\n", lte.rxCounter);

        if(AsciiToHex(lte.rxBuffer, lte.rxHexBuffer, lte.rxCounter) == 0)
        {
            debug_printf("rxHexBuffer =");

            for(uint8_t i = 0; i < lte.rxCounter / 2; i++)
            {
                debug_printf(" %x", lte.rxHexBuffer[i]);
            }

            if(lte.rxHexBuffer[0] == 0xE5 && lte.rxHexBuffer[1] == 0x5E)
            {
                printf("##Header Right##\r\n");
                EepromCtrl(lte.rxHexBuffer);
            }
            else if((lte.rxHexBuffer[0] == collectorIDBuffer[0]) && (lte.rxHexBuffer[1] == collectorIDBuffer[1]) && (lte.rxHexBuffer[2] == collectorIDBuffer[2]) && (lte.rxHexBuffer[3] == collectorIDBuffer[3]))
            {
                printf("##CollectorID Right##\r\n");
                FunctionCtrl(lte.rxHexBuffer);
            }
        }

        memset(&lte, 0, sizeof(lte));
        LED2_OFF();
    }

  /* USER CODE END StartUsartRxCmdTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/*===========================================================================
* EepromCtrl(uint8_t *command) => handle PC command                    	*
============================================================================*/
void EepromCtrl(uint8_t *command)
{
    uint32_t address, data;

    address = (uint32_t)(0x0000FF00 & command[3] << 8) + (uint32_t)(0x000000FF & command[4]);
    data = (uint32_t)(0xFF000000 & command[5] << 24) + (uint32_t)(0x00FF0000 & command[6] << 16) + (uint32_t)(0x0000FF00 & command[7] << 8) + (uint32_t)(0x000000FF & command[8]);

    if(command[2] == 0x01)//write
    {
        DATAEEPROM_Program(EEPROM_START_ADDR + address, data);
        printf("##Write Memory Complete##\r\n");
    }
    else if(command[2] == 0x02)//read
    {
        printf("%08x\n", DATAEEPROM_Read(EEPROM_START_ADDR + address));
        printf("##Read Memory Complete##\r\n");
    }
    else if(command[2] == 0x03)//erase
    {
        DATAEEPROM_Program(EEPROM_START_ADDR + address, 0);
        printf("##Erase Memory Complete##\r\n");
    }
}

/*===========================================================================
* FunctionCtrl(uint8_t *command) => handle PC command                    	*
============================================================================*/
void FunctionCtrl(uint8_t *command)
{
    uint8_t timeBuffer[3];
    uint8_t dateBuffer[4];

    /*A8:configure time information*/
    /*A9:read time information*/
    if(command[4] == 0xA0)
    {
        printf("##System Reset##\r\n");
        __disable_irq();
        NVIC_SystemReset();
    }
    else if(command[4] == 0xA8)
    {
        for(uint8_t i = 0; i < 3; i++)
        {
            timeBuffer[i] = command[9 + i];
        }

        for(uint8_t i = 0; i < 4; i++)
        {
            dateBuffer[i] = command[5 + i];
        }

        SetRTC(timeBuffer, dateBuffer);
				GetRTC(timeBuffer, dateBuffer);
//        SendToCloud(command[4], _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + _UTC_TIME_SIZE + _TAIL_SIZE);
    }
    else if(command[4] == 0xA9)
    {
				GetRTC(timeBuffer, dateBuffer);
//        SendToCloud(command[4], _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + _UTC_TIME_SIZE + _TAIL_SIZE);
    }
    else
        printf("##FunctionID Unused##\r\n");
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
        LED1_ON();
        GetRTC(timeBuffer, dateBuffer);

        strcatArray(sendToCloudBuffer, collectorIDBuffer, 0, 0);
        sendFunctionID = 0xB0;
        strcatArray(sendToCloudBuffer, &sendFunctionID, _COLLECTOR_ID_SIZE, 0);
        strcatArray(sendToCloudBuffer, cc1101.recvBuffer, _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE, length - sizeof(cc1101.crcValue));
        strcatArray(sendToCloudBuffer, &cc1101.rssi, _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + length - sizeof(cc1101.crcValue), 0);
        strcatArray(sendToCloudBuffer, dateBuffer, _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + length - sizeof(cc1101.crcValue) + sizeof(cc1101.rssi), 0);
        strcatArray(sendToCloudBuffer, timeBuffer, _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + length - sizeof(cc1101.crcValue) + sizeof(cc1101.rssi) + sizeof(dateBuffer), 0);

        cc1101.crcValue = ~HAL_CRC_Calculate(&hcrc, (uint32_t *)sendToCloudBuffer, (uint32_t)(_COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + length - sizeof(cc1101.crcValue) + sizeof(cc1101.rssi) + _UTC_TIME_SIZE));
        debug_printf("BufferLength = %d\n", _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + length - sizeof(cc1101.crcValue) + sizeof(cc1101.rssi) + _UTC_TIME_SIZE);
        debug_printf("crcValue = %x\n", cc1101.crcValue);
        crcBuffer[0] = (uint8_t)(0xFF & cc1101.crcValue >> 24);
        crcBuffer[1] = (uint8_t)(0xFF & cc1101.crcValue >> 16);
        crcBuffer[2] = (uint8_t)(0xFF & cc1101.crcValue >> 8);
        crcBuffer[3] = (uint8_t)(0xFF & cc1101.crcValue);
        strcatArray(sendToCloudBuffer, crcBuffer, _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + length - sizeof(cc1101.crcValue) + sizeof(cc1101.rssi) + _UTC_TIME_SIZE, 0);
        strcatArray(sendToCloudBuffer, tailBuffer, _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + length + sizeof(cc1101.rssi) + _UTC_TIME_SIZE, 0);

        ec600x_usart_send_data((uint8_t*)sendToCloudBuffer, _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + length + sizeof(cc1101.rssi) + _UTC_TIME_SIZE + _TAIL_SIZE);

//        for(uint8_t i = 0; i < _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + length + sizeof(cc1101.rssi) + _UTC_TIME_SIZE + _TAIL_SIZE; i++)
//        {
//            sendToCloudBuffer[i] = i;
//        }

        lte_lpuart_send_data((uint8_t*)sendToCloudBuffer, _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + length + sizeof(cc1101.rssi) + _UTC_TIME_SIZE + _TAIL_SIZE);

//        for(uint8_t i = 0; i < _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + length + sizeof(cc1101.rssi) + _UTC_TIME_SIZE; i++)
//        {
//            printf("%02x ", sendToCloudBuffer[i]);
//        }
//        printf("\r\n");
        LED1_OFF();
    }
    else if(functionID == 0xA8 || functionID == 0xA9)
    {
        LED1_ON();
        GetRTC(timeBuffer, dateBuffer);
        strcatArray(sendToCloudBuffer, collectorIDBuffer, 0, 0);
        sendFunctionID = 0xB0 + (0x0F & functionID);
        strcatArray(sendToCloudBuffer, &sendFunctionID, _COLLECTOR_ID_SIZE, 0);
        strcatArray(sendToCloudBuffer, dateBuffer, _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE, 0);
        strcatArray(sendToCloudBuffer, timeBuffer, _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + sizeof(dateBuffer), 0);
        strcatArray(sendToCloudBuffer, tailBuffer, _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + _UTC_TIME_SIZE, 0);

        ec600x_usart_send_data(sendToCloudBuffer, length);
        lte_lpuart_send_data(sendToCloudBuffer, length);

//        for(uint8_t i = 0; i < _COLLECTOR_ID_SIZE + _FUNCTION_ID_SIZE + _UTC_TIME_SIZE; i++)
//        {
//            printf("%02x ", sendToCloudBuffer[i]);
//        }
//        printf("\r\n");
        LED1_OFF();
    }
    else if(functionID == 0x01)
    {
        printf("receive error or address filtering fail\r\n");
    }
    else if(functionID == 0x02)
    {
        printf("rfid receive crc error\r\n");
    }

}
/* USER CODE END Application */

