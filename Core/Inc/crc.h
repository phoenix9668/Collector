/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    crc.h
  * @brief   This file contains all the function prototypes for
  *          the crc.c file
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
#ifndef __CRC_H__
#define __CRC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern CRC_HandleTypeDef hcrc;

/* USER CODE BEGIN Private defines */
typedef enum { CRC_INPUTDATA_FORMAT_WORDS, CRC_INPUTDATA_FORMAT_BYTES, CRC_INPUTDATA_FORMAT_HALFWORDS } InputDataFormat;
/* USER CODE END Private defines */

void MX_CRC_Init(void);

/* USER CODE BEGIN Prototypes */
uint32_t CRC32_Check(uint8_t pBuffer[], uint32_t BufferLength, InputDataFormat inputDataFormat);
uint32_t CRC32_Calculate(uint8_t pBuffer[], uint32_t BufferLength, InputDataFormat inputDataFormat);
uint32_t HAL_CRC_Calculate_U(CRC_HandleTypeDef *hcrc, uint32_t pBuffer[], uint32_t BufferLength, InputDataFormat inputDataFormat);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __CRC_H__ */

