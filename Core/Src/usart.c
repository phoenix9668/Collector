/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
lte_usart_rx_dma_index_t lte_usart_rx_dma_index;
lte_t lte;

#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/* USER CODE END 0 */

/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  /**USART3 GPIO Configuration
  PB10   ------> USART3_TX
  PB11   ------> USART3_RX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_10|LL_GPIO_PIN_11;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USART3 DMA Init */

  /* USART3_RX Init */
  LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_1, LL_DMA_CHANNEL_4);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_1, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_1, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_STREAM_1, LL_DMA_MODE_CIRCULAR);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_1, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_1, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_1, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_1, LL_DMA_MDATAALIGN_BYTE);

  LL_DMA_DisableFifoMode(DMA1, LL_DMA_STREAM_1);

  /* USART3 interrupt Init */
  NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),8, 0));
  NVIC_EnableIRQ(USART3_IRQn);

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  USART_InitStruct.BaudRate = 115200;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART3, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(USART3);
  LL_USART_Enable(USART3);
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/* USER CODE BEGIN 1 */
//##################################################################################################################
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int fputc(int ch,FILE *f)
{
	_LTE_USART->DR = ch;
	while(!(_LTE_USART->SR&USART_SR_TXE))
	{;}
	return ch;
}
//##################################################################################################################
void lte_usart_init(void)
{
	LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_1, (uint32_t)&_LTE_USART->DR);
	LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_1, (uint32_t)lte_usart_rx_dma_buffer);
	LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_1, ARRAY_LEN(lte_usart_rx_dma_buffer));

	/* Enable HT & TC interrupts */
	LL_DMA_EnableIT_HT(DMA1, LL_DMA_STREAM_1);
	LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_1);

	LL_USART_EnableDMAReq_RX(_LTE_USART);
	LL_USART_EnableIT_IDLE(_LTE_USART);
	
	/* Enable DMA */
	LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_1);
}
//##################################################################################################################
void lte_usart_deinit(void)
{
	/* Disable HT & TC interrupts */
	LL_DMA_DisableIT_HT(DMA1, LL_DMA_STREAM_1);
	LL_DMA_DisableIT_TC(DMA1, LL_DMA_STREAM_1);

	LL_USART_DisableDMAReq_RX(_LTE_USART);
	LL_USART_DisableIT_IDLE(_LTE_USART);
	
	/* Disable DMA */
	LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_1);
}
//##################################################################################################################
/**
 * \brief           Process received data over UART
 * Data are written to RX ringbuffer for application processing at latter stage
 * \param[in]       data: Data to process
 * \param[in]       len: Length in units of bytes
 */
bool lte_usart_process_data(const void* data, size_t len, uint8_t mode) {
	const uint8_t* d = data;
	if ((mode == 1) && (lte_usart_rx_dma_index.DMAHTIndex == true)){
		for (uint8_t i=0; i<len; i++, ++d)
				lte.rxBuffer[i] = *d;
		lte.rxCounter += len;
		return true;
	}
	if ((mode == 2) && (lte_usart_rx_dma_index.DMATCIndex == true)){
		for (uint8_t i=0; i<len; i++, ++d)
				lte.rxBuffer[i] = *d;
		lte.rxCounter += len;
		return true;
	}
	if (lte_usart_rx_dma_index.UARTIdleIndex == true){
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
void lte_usart_rx_check(void) {
    static size_t old_pos;
    size_t pos;

    /* Calculate current position in buffer */
    pos = ARRAY_LEN(lte_usart_rx_dma_buffer) - LL_DMA_GetDataLength(DMA1, LL_DMA_STREAM_1);
    if (pos != old_pos) {                       /* Check change in received data */
        if (pos > old_pos) {                    /* Current position is over previous one */
            /* We are in "linear" mode */
            /* Process data directly by subtracting "pointers" */
            lte_usart_process_data(&lte_usart_rx_dma_buffer[old_pos], pos - old_pos, 1);
        } else {
            /* We are in "overflow" mode */
            /* First process data to the end of buffer */\
            lte_usart_process_data(&lte_usart_rx_dma_buffer[old_pos], ARRAY_LEN(lte_usart_rx_dma_buffer) - old_pos, 2);
            /* Check and continue with beginning of buffer */
            if (pos > 0) {
              lte_usart_process_data(&lte_usart_rx_dma_buffer[0], pos, 3);
            }
        }
        old_pos = pos;                          /* Save current position as old */
				memset(&lte_usart_rx_dma_index,0,sizeof(lte_usart_rx_dma_index));
    }
}
//##################################################################################################################
/**
 * \brief           Process received data over UART
 * \note            Either process them directly or copy to other bigger buffer
 * \param[in]       data: Data to process
 * \param[in]       len: Length in units of bytes
 */
void lte_usart_send_data(const void* data, size_t len) {
	const uint8_t* d = data;
	/*
	 * This function is called on DMA TC and HT events, aswell as on UART IDLE (if enabled) line event.
	 * 
	 * For the sake of this example, function does a loop-back data over UART in polling mode.
	 * Check ringbuff RX-based example for implementation with TX & RX DMA transfer.
	 */

	for (; len > 0; --len, ++d) {
			LL_USART_TransmitData8(_LTE_USART, *d);
			while (!LL_USART_IsActiveFlag_TXE(_LTE_USART)) {}
	}
	while (!LL_USART_IsActiveFlag_TC(_LTE_USART)) {}
}
//##################################################################################################################
/**
 * \brief           Send string to USART
 * \param[in]       str: String to send
 */
void lte_usart_send_string(const char* str) {
    lte_usart_send_data((uint8_t*)str, strlen(str));
}
//##################################################################################################################
void MX_USART3_UART_DeInit(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  /* Peripheral clock disable */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  /**USART3 GPIO Configuration
  PB10   ------> USART3_TX
  PB11   ------> USART3_RX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_11;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	LL_USART_Disable(USART3);
  LL_USART_DeInit(USART3);
}
//##################################################################################################################
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
