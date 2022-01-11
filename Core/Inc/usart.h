/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
#define	_LTE_RXSIZE        128    //  GPS-command rx buffer size
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

typedef struct 
{
	bool      UARTIdleIndex;
	bool      DMAHTIndex;
	bool      DMATCIndex;
	
}lte_usart_rx_dma_index_t;

extern lte_usart_rx_dma_index_t lte_usart_rx_dma_index;
//##################################################################################################################
typedef struct 
{
  uint8_t   rxBuffer[_LTE_RXSIZE];
	uint16_t	rxCounter;
	bool      rxIndex;
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
