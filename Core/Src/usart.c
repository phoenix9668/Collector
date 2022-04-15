/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "cmsis_os.h"
extern osSemaphoreId usartIdleBinarySemHandle;
extern osSemaphoreId dmaHTBinarySemHandle;
extern osSemaphoreId dmaTCBinarySemHandle;
lte_t lte;

#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/* USER CODE END 0 */

/* LPUART1 init function */

void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  LL_LPUART_InitTypeDef LPUART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_LPUART1);

  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
  /**LPUART1 GPIO Configuration
  PB10   ------> LPUART1_TX
  PB11   ------> LPUART1_RX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_11;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* LPUART1 DMA Init */

  /* LPUART1_RX Init */
  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_3, LL_DMA_REQUEST_5);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_3, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MODE_CIRCULAR);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MDATAALIGN_BYTE);

  /* LPUART1 interrupt Init */
  NVIC_SetPriority(LPUART1_IRQn, 3);
  NVIC_EnableIRQ(LPUART1_IRQn);

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  LPUART_InitStruct.BaudRate = 115200;
  LPUART_InitStruct.DataWidth = LL_LPUART_DATAWIDTH_8B;
  LPUART_InitStruct.StopBits = LL_LPUART_STOPBITS_1;
  LPUART_InitStruct.Parity = LL_LPUART_PARITY_NONE;
  LPUART_InitStruct.TransferDirection = LL_LPUART_DIRECTION_TX_RX;
  LPUART_InitStruct.HardwareFlowControl = LL_LPUART_HWCONTROL_NONE;
  LL_LPUART_Init(LPUART1, &LPUART_InitStruct);
  /* USER CODE BEGIN LPUART1_Init 2 */

  /* USER CODE END LPUART1_Init 2 */

}
/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  /**USART1 GPIO Configuration
  PA9   ------> USART1_TX
  PA10   ------> USART1_RX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  USART_InitStruct.BaudRate = 115200;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART1, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(USART1);
  LL_USART_Enable(USART1);
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/* USER CODE BEGIN 1 */
//##################################################################################################################
/**
  * @brief  This function Enable LPUART1
  * @param  None
  * @retval None
  */
void Enable_LPUART1(void)
{
  /* Enable LPUART1 */
	LL_LPUART_Enable(LPUART1);
	
	/* Polling USART initialisation */
  while((!(LL_LPUART_IsActiveFlag_TEACK(LPUART1))) || (!(LL_LPUART_IsActiveFlag_REACK(LPUART1))))
  {}
}
//##################################################################################################################
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int fputc(int ch,FILE *f)
{
	_LTE_LPUART->TDR = ch;
	while(!(_LTE_LPUART->ISR&USART_ISR_TXE))
	{;}
	return ch;
}
//##################################################################################################################
void lte_lpuart_init(void)
{
	LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_3, (uint32_t)&_LTE_LPUART->RDR);
	LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_3, (uint32_t)lte_lpuart_rx_dma_buffer);
	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, ARRAY_LEN(lte_lpuart_rx_dma_buffer));

	/* Enable HT & TC interrupts */
	LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_3);
	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_3);

	LL_USART_EnableDMAReq_RX(_LTE_LPUART);
	LL_USART_EnableIT_IDLE(_LTE_LPUART);
	
	/* Enable DMA */
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
}
//##################################################################################################################
void lte_lpuart_deinit(void)
{
	/* Disable HT & TC interrupts */
	LL_DMA_DisableIT_HT(DMA1, LL_DMA_CHANNEL_3);
	LL_DMA_DisableIT_TC(DMA1, LL_DMA_CHANNEL_3);

	LL_USART_DisableDMAReq_RX(_LTE_LPUART);
	LL_USART_DisableIT_IDLE(_LTE_LPUART);
	
	/* Disable DMA */
	LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_3);
}
//##################################################################################################################
/**
 * \brief           Process received data over UART
 * Data are written to RX ringbuffer for application processing at latter stage
 * \param[in]       data: Data to process
 * \param[in]       len: Length in units of bytes
 */
bool lte_lpuart_process_data(const void* data, size_t len, uint8_t mode) {
	const uint8_t* d = data;
	if ((mode == 1) && (osSemaphoreWait(dmaHTBinarySemHandle, 0) == osOK)){
		for (uint8_t i=0; i<len; i++, ++d)
				lte.rxBuffer[i] = *d;
		lte.rxCounter += len;
		return true;
	}
	if ((mode == 2) && (osSemaphoreWait(dmaTCBinarySemHandle, 0) == osOK)){
		for (uint8_t i=0; i<len; i++, ++d)
				lte.rxBuffer[i] = *d;
		lte.rxCounter += len;
		return true;
	}
	if (osSemaphoreWait(usartIdleBinarySemHandle, 0) == osOK){
		if (mode == 1){
			if (lte.rxCounter == 0){
				for (uint8_t i=0; i<len; i++, ++d)
					lte.rxBuffer[i] = *d;
				lte.rxCounter += len;
			}
			else{
				for (uint8_t i=0; i<len; i++, ++d)
					lte.rxBuffer[lte.rxCounter+i] = *d;
				lte.rxCounter += len;
			}
			return true;
		}
		if (mode == 3){
			for (uint8_t i=0; i<len; i++, ++d)
				lte.rxBuffer[lte.rxCounter+i] = *d;
			lte.rxCounter += len;
			return true;
		}
	}
	return false;
}
//##################################################################################################################
/**
 * \brief           Check for new data received with DMA
 */
void lte_lpuart_rx_check(void) {
    static size_t old_pos;
    size_t pos;

    /* Calculate current position in buffer */
    pos = ARRAY_LEN(lte_lpuart_rx_dma_buffer) - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_3);
    if (pos != old_pos) {                       /* Check change in received data */
        if (pos > old_pos) {                    /* Current position is over previous one */
            /* We are in "linear" mode */
            /* Process data directly by subtracting "pointers" */
            lte_lpuart_process_data(&lte_lpuart_rx_dma_buffer[old_pos], pos - old_pos, 1);
        } else {
            /* We are in "overflow" mode */
            /* First process data to the end of buffer */\
            lte_lpuart_process_data(&lte_lpuart_rx_dma_buffer[old_pos], ARRAY_LEN(lte_lpuart_rx_dma_buffer) - old_pos, 2);
            /* Check and continue with beginning of buffer */
            if (pos > 0) {
              lte_lpuart_process_data(&lte_lpuart_rx_dma_buffer[0], pos, 3);
            }
        }
        old_pos = pos;                          /* Save current position as old */
    }
}
//##################################################################################################################
/**
 * \brief           Process received data over UART
 * \note            Either process them directly or copy to other bigger buffer
 * \param[in]       data: Data to process
 * \param[in]       len: Length in units of bytes
 */
void lte_lpuart_send_data(const void* data, size_t len) {
	const uint8_t* d = data;
	/*
	 * This function is called on DMA TC and HT events, aswell as on UART IDLE (if enabled) line event.
	 * 
	 * For the sake of this example, function does a loop-back data over UART in polling mode.
	 * Check ringbuff RX-based example for implementation with TX & RX DMA transfer.
	 */

	for (; len > 0; --len, ++d) {
			LL_USART_TransmitData8(_LTE_LPUART, *d);
			while (!LL_USART_IsActiveFlag_TXE(_LTE_LPUART)) {}
	}
	while (!LL_USART_IsActiveFlag_TC(_LTE_LPUART)) {}
}
//##################################################################################################################
/**
 * \brief           Process received data over UART
 * \note            Either process them directly or copy to other bigger buffer
 * \param[in]       data: Data to process
 * \param[in]       len: Length in units of bytes
 */
void ec600x_usart_send_data(const void* data, size_t len) {
	const uint8_t* d = data;
	/*
	 * This function is called on DMA TC and HT events, aswell as on UART IDLE (if enabled) line event.
	 * 
	 * For the sake of this example, function does a loop-back data over UART in polling mode.
	 * Check ringbuff RX-based example for implementation with TX & RX DMA transfer.
	 */

	for (; len > 0; --len, ++d) {
			LL_USART_TransmitData8(_EC600X_USART, *d);
			while (!LL_USART_IsActiveFlag_TXE(_EC600X_USART)) {}
	}
	while (!LL_USART_IsActiveFlag_TC(_EC600X_USART)) {}
}
//##################################################################################################################
/**
 * \brief           Send string to USART
 * \param[in]       str: String to send
 */
void lte_lpuart_send_string(const char* str) {
    lte_lpuart_send_data((uint8_t*)str, strlen(str));
}
//##################################################################################################################
/**
 * \brief           Send string to USART
 * \param[in]       str: String to send
 */
void ec600x_usart_send_string(const char* str) {
    ec600x_usart_send_data((uint8_t*)str, strlen(str));
}
//##################################################################################################################
/* USER CODE END 1 */
