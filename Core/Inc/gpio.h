/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define LED_GREEN_ON()			  HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET)
#define LED_GREEN_OFF()			  HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET)
#define LED_GREEN_TOG()			  HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin)

#define LED1_ON()					    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET)
#define LED1_OFF()					  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET)
#define LED1_TOG()					  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin)

#define LED2_ON()					    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET)
#define LED2_OFF()					  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET)
#define LED2_TOG()					  HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin)

#define TX_EN_LOW()			      HAL_GPIO_WritePin(TX_EN_GPIO_Port, TX_EN_Pin, GPIO_PIN_RESET)
#define TX_EN_HIGH()			    HAL_GPIO_WritePin(TX_EN_GPIO_Port, TX_EN_Pin, GPIO_PIN_SET)
#define RX_EN_LOW()			      HAL_GPIO_WritePin(RX_EN_GPIO_Port, RX_EN_Pin, GPIO_PIN_RESET)
#define RX_EN_HIGH()		      HAL_GPIO_WritePin(RX_EN_GPIO_Port, RX_EN_Pin, GPIO_PIN_SET)

#define USR4G_POWER_KEY_OFF()		  HAL_GPIO_WritePin(USR4G_POWER_KEY_GPIO_Port, USR4G_POWER_KEY_Pin, GPIO_PIN_RESET)
#define USR4G_POWER_KEY_ON()		  HAL_GPIO_WritePin(USR4G_POWER_KEY_GPIO_Port, USR4G_POWER_KEY_Pin, GPIO_PIN_SET)
#define USR4G_POWER_KEY_TOG()		  HAL_GPIO_TogglePin(USR4G_POWER_KEY_GPIO_Port, USR4G_POWER_KEY_Pin)

//#define USR4G_RESET_OFF()		      HAL_GPIO_WritePin(USR4G_RESET_GPIO_Port, USR4G_RESET_Pin, GPIO_PIN_RESET)
//#define USR4G_RESET_ON()		      HAL_GPIO_WritePin(USR4G_RESET_GPIO_Port, USR4G_RESET_Pin, GPIO_PIN_SET)
//#define USR4G_RESET_TOG()		      HAL_GPIO_TogglePin(MOD_RESET_GPIO_Port, USR4G_RESET_Pin)

//#define USR4G_RELOAD_OFF()		    HAL_GPIO_WritePin(USR4G_RELOAD_GPIO_Port, USR4G_RELOAD_Pin, GPIO_PIN_RESET)
//#define USR4G_RELOAD_ON()		      HAL_GPIO_WritePin(USR4G_RELOAD_GPIO_Port, USR4G_RELOAD_Pin, GPIO_PIN_SET)
//#define USR4G_RELOAD_TOG()		    HAL_GPIO_TogglePin(USR4G_RELOAD_GPIO_Port, USR4G_RELOAD_Pin)

#define CC1101_CSN_LOW()			HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET)
#define CC1101_CSN_HIGH()			HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET)

#define CC1101_MISO_READ()		HAL_GPIO_ReadPin(GPIOA, LL_GPIO_PIN_6)

#define FRAM_CSN_LOW()				HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET)
#define FRAM_CSN_HIGH()				HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET)
/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

