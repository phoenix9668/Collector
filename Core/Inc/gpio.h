/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
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
#define LED_COM_ON()					HAL_GPIO_WritePin(LED_COM_GPIO_Port, LED_COM_Pin, GPIO_PIN_RESET)
#define LED_COM_OFF()					HAL_GPIO_WritePin(LED_COM_GPIO_Port, LED_COM_Pin, GPIO_PIN_SET)
#define LED_COM_TOG()					HAL_GPIO_TogglePin(LED_COM_GPIO_Port, LED_COM_Pin)

#define LED_STA_ON()					HAL_GPIO_WritePin(LED_STA_GPIO_Port, LED_STA_Pin, GPIO_PIN_RESET)
#define LED_STA_OFF()					HAL_GPIO_WritePin(LED_STA_GPIO_Port, LED_STA_Pin, GPIO_PIN_SET)
#define LED_STA_TOG()					HAL_GPIO_TogglePin(LED_STA_GPIO_Port, LED_STA_Pin)

#define LED_RUN_ON()					HAL_GPIO_WritePin(LED_RUN_GPIO_Port, LED_RUN_Pin, GPIO_PIN_RESET)
#define LED_RUN_OFF()					HAL_GPIO_WritePin(LED_RUN_GPIO_Port, LED_RUN_Pin, GPIO_PIN_SET)
#define LED_RUN_TOG()					HAL_GPIO_TogglePin(LED_RUN_GPIO_Port, LED_RUN_Pin)

#define MOD_RESET_OFF()				HAL_GPIO_WritePin(MOD_RESET_GPIO_Port, MOD_RESET_Pin, GPIO_PIN_RESET)
#define MOD_RESET_ON()				HAL_GPIO_WritePin(MOD_RESET_GPIO_Port, MOD_RESET_Pin, GPIO_PIN_SET)
#define MOD_RESET_TOG()				HAL_GPIO_TogglePin(MOD_RESET_GPIO_Port, MOD_RESET_Pin)
#define MOD_GPRS_READ()   		HAL_GPIO_ReadPin(MOD_GPRS_GPIO_Port, MOD_GPRS_Pin)

#define CC1101_CSN_LOW()			HAL_GPIO_WritePin(SPI2_CS1_GPIO_Port, SPI2_CS1_Pin, GPIO_PIN_RESET)
#define CC1101_CSN_HIGH()			HAL_GPIO_WritePin(SPI2_CS1_GPIO_Port, SPI2_CS1_Pin, GPIO_PIN_SET)

#define CC1101_MISO_READ()		HAL_GPIO_ReadPin(GPIOB, LL_GPIO_PIN_14)

#define FRAM_CSN_LOW()				HAL_GPIO_WritePin(SPI2_CS2_GPIO_Port, SPI2_CS2_Pin, GPIO_PIN_RESET)
#define FRAM_CSN_HIGH()				HAL_GPIO_WritePin(SPI2_CS2_GPIO_Port, SPI2_CS2_Pin, GPIO_PIN_SET)
/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
