/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    lptim.c
  * @brief   This file provides code for the configuration
  *          of the LPTIM instances.
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
#include "lptim.h"

/* USER CODE BEGIN 0 */
#include "sgm58031.h"
ITStatus secITStatus;
/* USER CODE END 0 */

LPTIM_HandleTypeDef hlptim1;

/* LPTIM1 init function */
void MX_LPTIM1_Init(void)
{

    /* USER CODE BEGIN LPTIM1_Init 0 */

    /* USER CODE END LPTIM1_Init 0 */

    /* USER CODE BEGIN LPTIM1_Init 1 */

    /* USER CODE END LPTIM1_Init 1 */
    hlptim1.Instance = LPTIM1;
    hlptim1.Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
    hlptim1.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV128;
    hlptim1.Init.Trigger.Source = LPTIM_TRIGSOURCE_SOFTWARE;
    hlptim1.Init.OutputPolarity = LPTIM_OUTPUTPOLARITY_HIGH;
    hlptim1.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
    hlptim1.Init.CounterSource = LPTIM_COUNTERSOURCE_INTERNAL;

    if (HAL_LPTIM_Init(&hlptim1) != HAL_OK)
    {
        Error_Handler();
    }

    /* USER CODE BEGIN LPTIM1_Init 2 */

    /* USER CODE END LPTIM1_Init 2 */

}

void HAL_LPTIM_MspInit(LPTIM_HandleTypeDef* lptimHandle)
{

    if(lptimHandle->Instance == LPTIM1)
    {
        /* USER CODE BEGIN LPTIM1_MspInit 0 */

        /* USER CODE END LPTIM1_MspInit 0 */
        /* LPTIM1 clock enable */
        __HAL_RCC_LPTIM1_CLK_ENABLE();

        /* LPTIM1 interrupt Init */
        HAL_NVIC_SetPriority(LPTIM1_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ(LPTIM1_IRQn);
        /* USER CODE BEGIN LPTIM1_MspInit 1 */

        /* USER CODE END LPTIM1_MspInit 1 */
    }
}

void HAL_LPTIM_MspDeInit(LPTIM_HandleTypeDef* lptimHandle)
{

    if(lptimHandle->Instance == LPTIM1)
    {
        /* USER CODE BEGIN LPTIM1_MspDeInit 0 */

        /* USER CODE END LPTIM1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_LPTIM1_CLK_DISABLE();

        /* LPTIM1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(LPTIM1_IRQn);
        /* USER CODE BEGIN LPTIM1_MspDeInit 1 */

        /* USER CODE END LPTIM1_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

/**
  * @brief  Autoreload match callback in non-blocking mode.
  * @param  hlptim LPTIM handle
  * @retval None
  */
void HAL_LPTIM_AutoReloadMatchCallback(LPTIM_HandleTypeDef *hlptim)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hlptim);
    secITStatus = SET;
}

/* USER CODE END 1 */
