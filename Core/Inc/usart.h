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
#include "lwrb.h"
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
//##################################################################################################################
#define _LTE_LPUART        LPUART1
#define	_LTE_RXSIZE        128     //  LTE-command rx buffer size
#define	_LTE_RBSIZE        256     //  LTE-command ring buffer size
#define _EC600X_USART      USART1
//##################################################################################################################
/**
 * \brief           Calculate length of statically allocated array
 */
#define ARRAY_LEN(x)       (sizeof(x) / sizeof((x)[0]))
//##################################################################################################################
/**
 * \brief           USART RX buffer for DMA to transfer every received byte RX
 * \note            Contains raw data that are about to be processed by different events
 */
extern uint8_t lte_lpuart_rx_dma_buffer[_LTE_RXSIZE];

/**
 * \brief           Ring buffer instance for RX data
 */
extern lwrb_t lte_lpuart_rx_rb;

/**
 * \brief           Ring buffer data array for RX DMA
 */
extern uint8_t lte_lpuart_rx_rb_data[_LTE_RBSIZE];

/**
 * \brief           Ring buffer instance for TX data
 */
extern lwrb_t lte_lpuart_tx_rb;

/**
 * \brief           Ring buffer data array for TX DMA
 */
extern uint8_t lte_lpuart_tx_rb_data[_LTE_RBSIZE];

/**
 * \brief           Length of currently active TX DMA transfer
 */
extern volatile size_t lte_lpuart_tx_dma_current_len;

//##################################################################################################################
typedef struct
{
  uint8_t   rxBuffer[_LTE_RXSIZE];
	uint16_t	rxCounter;
}lte_t;

extern lte_t lte;
//##################################################################################################################
/* USER CODE END Private defines */

void MX_LPUART1_UART_Init(void);
void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */
/* USART related functions */
void Enable_LPUART1(void);
void lte_lpuart_init(void);
void lte_lpuart_deinit(void);
void lte_lpuart_rx_check(void);
void lte_lpuart_send_string(const char* str);
uint8_t lte_lpuart_start_tx_dma_transfer(void);

void ec600x_usart_send_data(const void* data, size_t len);
void ec600x_usart_send_string(const char* str);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

