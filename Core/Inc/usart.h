/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
//##################################################################################################################
#define _LTE_USART         USART3
#define	_LTE_RXSIZE        128    //  LTE-command rx buffer size
//##################################################################################################################
/**
 * \brief           Calculate length of statically allocated array
 */
#define ARRAY_LEN(x)       (sizeof(x) / sizeof((x)[0]))
//##################################################################################################################
/**
 * \brief           Buffer for USART DMA
 * \note            Contains RAW unprocessed data received by UART and transfered by DMA
 */
static uint8_t lte_usart_rx_dma_buffer[_LTE_RXSIZE];

//##################################################################################################################
typedef struct 
{
  uint8_t   rxBuffer[_LTE_RXSIZE];
	uint16_t	rxCounter;
}lte_t;

extern lte_t lte;
//##################################################################################################################
/* USER CODE END Private defines */

void MX_USART3_UART_Init(void);

/* USER CODE BEGIN Prototypes */
/* USART related functions */
void lte_usart_init(void);
void lte_usart_deinit(void);
void lte_usart_rx_check(void);
void lte_usart_send_data(const void* data, size_t len);
void lte_usart_send_string(const char* str);

void MX_USART3_UART_DeInit(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

