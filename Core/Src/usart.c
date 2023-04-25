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

uint8_t ec600x_usart_rx_dma_buffer[_EC600X_RXSIZE];
lwrb_t ec600x_usart_rx_rb;
uint8_t ec600x_usart_rx_rb_data[_EC600X_RBSIZE];
lwrb_t ec600x_usart_tx_rb;
uint8_t ec600x_usart_tx_rb_data[_EC600X_RBSIZE];
volatile size_t ec600x_usart_tx_dma_current_len;
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

  /* USART1 DMA Init */

  /* USART1_RX Init */
  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_3, LL_DMA_REQUEST_3);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_3, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MODE_CIRCULAR);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MDATAALIGN_BYTE);

  /* USART1_TX Init */
  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_2, LL_DMA_REQUEST_3);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_2, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_2, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_2, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_2, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_2, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_2, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_2, LL_DMA_MDATAALIGN_BYTE);

  /* USART1 interrupt Init */
  NVIC_SetPriority(USART1_IRQn, 3);
  NVIC_EnableIRQ(USART1_IRQn);

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
//int fputc(int ch, FILE *f)
//{
//    _EC600X_USART->TDR = ch;

//    while(!(_EC600X_USART->ISR & USART_ISR_TXE))
//    {;}

//    return ch;
//}

int fputc(int ch, FILE *f)
{
    _LTE_LPUART->TDR = ch;

    while(!(_LTE_LPUART->ISR & USART_ISR_TXE))
    {;}

    return ch;
}
//##################################################################################################################
void ec600x_usart_init(void)
{
    /* Initialize ringbuff for TX & RX */
    lwrb_init(&ec600x_usart_tx_rb, ec600x_usart_tx_rb_data, sizeof(ec600x_usart_tx_rb_data));
    lwrb_init(&ec600x_usart_rx_rb, ec600x_usart_rx_rb_data, sizeof(ec600x_usart_rx_rb_data));

    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_3, LL_USART_DMA_GetRegAddr(_EC600X_USART, LL_USART_DMA_REG_DATA_RECEIVE));
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_3, (uint32_t)ec600x_usart_rx_dma_buffer);
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, ARRAY_LEN(ec600x_usart_rx_dma_buffer));
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_2, LL_USART_DMA_GetRegAddr(_EC600X_USART, LL_USART_DMA_REG_DATA_TRANSMIT));

    /* Enable DMA RX HT & TC interrupts */
    LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_3);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_3);
    /* Enable DMA TX TC interrupts */
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_2);

    LL_USART_ConfigAsyncMode(_EC600X_USART);
    LL_USART_EnableDMAReq_RX(_EC600X_USART);
    LL_USART_EnableDMAReq_TX(_EC600X_USART);
    LL_USART_EnableIT_IDLE(_EC600X_USART);

    /* Enable DMA RX*/
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
}
//##################################################################################################################
void ec600x_usart_deinit(void)
{
    /* Disable DMA RX HT & TC interrupts */
    LL_DMA_DisableIT_HT(DMA1, LL_DMA_CHANNEL_3);
    LL_DMA_DisableIT_TC(DMA1, LL_DMA_CHANNEL_3);
    /* Disable DMA TX TC interrupts */
    LL_DMA_DisableIT_TC(DMA1, LL_DMA_CHANNEL_2);

    LL_USART_DisableDMAReq_RX(_EC600X_USART);
    LL_USART_DisableDMAReq_TX(_EC600X_USART);
    LL_USART_DisableIT_IDLE(_EC600X_USART);

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
void ec600x_usart_process_data(const void* data, size_t len)
{
    lte.rxCounter += len;
    lwrb_write(&ec600x_usart_rx_rb, data, len);  /* Write data to receive buffer */
}
//##################################################################################################################
/**
 * \brief           Check for new data received with DMA
 */
void ec600x_usart_rx_check(void)
{
    static size_t old_pos;
    size_t pos;

    /* Calculate current position in buffer */
    pos = ARRAY_LEN(ec600x_usart_rx_dma_buffer) - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_3);

    if (pos != old_pos)                         /* Check change in received data */
    {
        if (pos > old_pos)                      /* Current position is over previous one */
        {
            /* We are in "linear" mode */
            /* Process data directly by subtracting "pointers" */
            ec600x_usart_process_data(&ec600x_usart_rx_dma_buffer[old_pos], pos - old_pos);
        }
        else
        {
            /* We are in "overflow" mode */
            /* First process data to the end of buffer */\
            ec600x_usart_process_data(&ec600x_usart_rx_dma_buffer[old_pos], ARRAY_LEN(ec600x_usart_rx_dma_buffer) - old_pos);

            /* Check and continue with beginning of buffer */
            if (pos > 0)
            {
                ec600x_usart_process_data(&ec600x_usart_rx_dma_buffer[0], pos);
            }
        }

        old_pos = pos;                          /* Save current position as old */
    }
}
//##################################################################################################################
/**
 * \brief           Check if DMA is active and if not try to send data
 * \return          `1` if transfer just started, `0` if on-going or no data to transmit
 */
uint8_t ec600x_usart_start_tx_dma_transfer(void)
{
    uint32_t primask;
    uint8_t started = 0;

    /*
     * First check if transfer is currently in-active,
     * by examining the value of usart_tx_dma_current_len variable.
     *
     * This variable is set before DMA transfer is started and cleared in DMA TX complete interrupt.
     *
     * It is not necessary to disable the interrupts before checking the variable:
     *
     * When usart_tx_dma_current_len == 0
     *    - This function is called by either application or TX DMA interrupt
     *    - When called from interrupt, it was just reset before the call,
     *         indicating transfer just completed and ready for more
     *    - When called from an application, transfer was previously already in-active
     *         and immediate call from interrupt cannot happen at this moment
     *
     * When usart_tx_dma_current_len != 0
     *    - This function is called only by an application.
     *    - It will never be called from interrupt with usart_tx_dma_current_len != 0 condition
     *
     * Disabling interrupts before checking for next transfer is advised
     * only if multiple operating system threads can access to this function w/o
     * exclusive access protection (mutex) configured,
     * or if application calls this function from multiple interrupts.
     *
     * This example assumes worst use case scenario,
     * hence interrupts are disabled prior every check
     */
    primask = __get_PRIMASK();
    __disable_irq();

    if (ec600x_usart_tx_dma_current_len == 0
            && (ec600x_usart_tx_dma_current_len = lwrb_get_linear_block_read_length(&ec600x_usart_tx_rb)) > 0)
    {
        /* Disable channel if enabled */
        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);

        /* Clear all flags */
        LL_DMA_ClearFlag_TC2(DMA1);
        LL_DMA_ClearFlag_HT2(DMA1);
        LL_DMA_ClearFlag_GI2(DMA1);
        LL_DMA_ClearFlag_TE2(DMA1);

        /* Prepare DMA data and length */
        LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_2, ec600x_usart_tx_dma_current_len);
        LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_2, (uint32_t)lwrb_get_linear_block_read_address(&ec600x_usart_tx_rb));

        /* Start transfer */
        LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);
        started = 1;
    }

    __set_PRIMASK(primask);
    return started;
}
//##################################################################################################################
/**
 * \brief           Send string over USART
 * \param[in]       str: String to send
 */
void ec600x_usart_send_string(const char* str)
{
    lwrb_write(&ec600x_usart_tx_rb, str, strlen(str));   /* Write data to transmit buffer */
    ec600x_usart_start_tx_dma_transfer();
}
//##################################################################################################################
/**
 * \brief           Process received data over UART
 * \note            Either process them directly or copy to other bigger buffer
 * \param[in]       data: Data to process
 * \param[in]       len: Length in units of bytes
 */
void ec600x_usart_send_data(const void* data, size_t len)
{
    const uint8_t* d = data;
    /*
     * This function is called on DMA TC and HT events, aswell as on UART IDLE (if enabled) line event.
     *
     * For the sake of this example, function does a loop-back data over UART in polling mode.
     * Check ringbuff RX-based example for implementation with TX & RX DMA transfer.
     */

    for (; len > 0; --len, ++d)
    {
        LL_USART_TransmitData8(_EC600X_USART, *d);

        while (!LL_USART_IsActiveFlag_TXE(_EC600X_USART)) {}
    }

    while (!LL_USART_IsActiveFlag_TC(_EC600X_USART)) {}
}
//##################################################################################################################
/**
 * \brief           Process received data over UART
 * \note            Either process them directly or copy to other bigger buffer
 * \param[in]       data: Data to process
 * \param[in]       len: Length in units of bytes
 */
void lte_lpuart_send_data(const void* data, size_t len)
{
    const uint8_t* d = data;
    /*
     * This function is called on DMA TC and HT events, aswell as on UART IDLE (if enabled) line event.
     *
     * For the sake of this example, function does a loop-back data over UART in polling mode.
     * Check ringbuff RX-based example for implementation with TX & RX DMA transfer.
     */

    for (; len > 0; --len, ++d)
    {
        LL_USART_TransmitData8(_LTE_LPUART, *d);

        while (!LL_USART_IsActiveFlag_TXE(_LTE_LPUART)) {}
    }

    while (!LL_USART_IsActiveFlag_TC(_LTE_LPUART)) {}
}
//##################################################################################################################
/**
 * \brief           Send string to USART
 * \param[in]       str: String to send
 */
void lte_lpuart_send_string(const char* str)
{
    lte_lpuart_send_data((uint8_t*)str, strlen(str));
}
//##################################################################################################################
/* USER CODE END 1 */
