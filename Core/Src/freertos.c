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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
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
/* Definitions for radioTask */
osThreadId_t radioTaskHandle;
uint32_t radioTaskBuffer[ 160 ];
osStaticThreadDef_t radioTaskControlBlock;
const osThreadAttr_t radioTask_attributes = {
  .name = "radioTask",
  .cb_mem = &radioTaskControlBlock,
  .cb_size = sizeof(radioTaskControlBlock),
  .stack_mem = &radioTaskBuffer[0],
  .stack_size = sizeof(radioTaskBuffer),
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for linkTask */
osThreadId_t linkTaskHandle;
uint32_t linkTaskBuffer[ 160 ];
osStaticThreadDef_t linkTaskControlBlock;
const osThreadAttr_t linkTask_attributes = {
  .name = "linkTask",
  .cb_mem = &linkTaskControlBlock,
  .cb_size = sizeof(linkTaskControlBlock),
  .stack_mem = &linkTaskBuffer[0],
  .stack_size = sizeof(linkTaskBuffer),
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for houseTask */
osThreadId_t houseTaskHandle;
uint32_t houseTaskBuffer[ 160 ];
osStaticThreadDef_t houseTaskControlBlock;
const osThreadAttr_t houseTask_attributes = {
  .name = "houseTask",
  .cb_mem = &houseTaskControlBlock,
  .cb_size = sizeof(houseTaskControlBlock),
  .stack_mem = &houseTaskBuffer[0],
  .stack_size = sizeof(houseTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for linkTxMutex */
osMutexId_t linkTxMutexHandle;
const osMutexAttr_t linkTxMutex_attributes = {
  .name = "linkTxMutex"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

extern void Radio_Task(void *argument);
extern void Link_Task(void *argument);
extern void Housekeeping_Task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 2 */
/* The FreeRTOS hooks declared above and the task bodies live in App/app_main.c
   and App/services. */
/* USER CODE END 2 */

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN 5 */

/* USER CODE END 5 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of linkTxMutex */
  linkTxMutexHandle = osMutexNew(&linkTxMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */

  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */

  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */

  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of radioTask */
  radioTaskHandle = osThreadNew(Radio_Task, NULL, &radioTask_attributes);

  /* creation of linkTask */
  linkTaskHandle = osThreadNew(Link_Task, NULL, &linkTask_attributes);

  /* creation of houseTask */
  houseTaskHandle = osThreadNew(Housekeeping_Task, NULL, &houseTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */

  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_Radio_Task */
/**
  * @brief  Function implementing the radioTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Radio_Task */
__weak void Radio_Task(void *argument)
{
  /* USER CODE BEGIN Radio_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Radio_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE END Application */

