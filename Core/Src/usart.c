/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
uint8_t RxBuffer[RXBUFFERSIZE];
__IO uint8_t RxCounter = 0;
__IO FlagStatus CommandState = RESET;

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

  /* USART3 interrupt Init */
  NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),3, 0));
  NVIC_EnableIRQ(USART3_IRQn);

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

}

/* USER CODE BEGIN 1 */
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int fputc(int ch,FILE *f)
{
	USART3->DR = ch;
	while(!(USART3->SR&USART_SR_TXE))
	{;}
	return ch;
}

void Activate_USART3_RXIT(void)
{
  /* Enable RXNE and Error interrupts */
  LL_USART_EnableIT_RXNE(USART3);
  LL_USART_EnableIT_ERROR(USART3);
}

/**
  * @brief  Send Txt information message on USART Tx line (to PC Com port).
  * @param  None
  * @retval None
  */
void PrintInfo(uint8_t *String, uint32_t Size)
{
  uint32_t index = 0;
  uint8_t *pchar = String;
  
  /* Send characters one per one, until last char to be sent */
  for (index = 0; index < Size; index++)
  {
    /* Wait for TXE flag to be raised */
    while (!LL_USART_IsActiveFlag_TXE(USART3))
    {
    }

    /* Write character in Transmit Data register.
       TXE flag is cleared by writing data in TDR register */
    LL_USART_TransmitData8(USART3, *pchar++);
  }

  /* Wait for TC flag to be raised for last char */
  while (!LL_USART_IsActiveFlag_TC(USART3))
  {
  }
}

/**
  * @brief  Function called from USART IRQ Handler when RXNE flag is set
  *         Function is in charge of reading character received on USART RX line.
  * @param  None
  * @retval None
  */
void USART_CharReception_Callback(void)
{
  /* Read Received character. RXNE flag is cleared by reading of RDR register */
  RxBuffer[RxCounter++] = LL_USART_ReceiveData8(USART3);
  /* Echo received character on TX */
  LL_USART_TransmitData8(USART3, RxBuffer[RxCounter-1]);
	/* Check if received value is corresponding to specific one : S or s */
  if (RxBuffer[RxCounter-2] == 0x0D && RxBuffer[RxCounter-1] == 0x0A)
  {
    /* Clear RxCounter : Expected character has been received */
    RxCounter = 0x00;
		CommandState = SET;
  }
}

/**
  * @brief  Function called in case of error detected in USART IT Handler
  * @param  None
  * @retval None
  */
void Error_Callback(void)
{
  __IO uint32_t isr_reg;

  /* Disable USARTx_IRQn */
  NVIC_DisableIRQ(USART3_IRQn);
  
  /* Error handling example :
    - Read USART ISR register to identify flag that leads to IT raising
    - Perform corresponding error handling treatment according to flag
  */
  isr_reg = LL_USART_ReadReg(USART3, SR);
  if (isr_reg & LL_USART_SR_NE)
  {
    /* case Noise Error flag is raised : ... */
			LL_USART_ClearFlag_NE(USART3);
			LL_USART_DeInit(USART3);
			MX_USART3_UART_Init();
			LL_USART_EnableIT_RXNE(USART3);
			LL_USART_EnableIT_ERROR(USART3);
			RxCounter = 0x00;
  }
	else if (isr_reg & LL_USART_SR_FE)
  {
    /* case Noise Error flag is raised : ... */
			LL_USART_ClearFlag_FE(USART3);
			LL_USART_DeInit(USART3);
			MX_USART3_UART_Init();
			LL_USART_EnableIT_RXNE(USART3);
			LL_USART_EnableIT_ERROR(USART3);
			RxCounter = 0x00;
  }
	else if (isr_reg & LL_USART_SR_ORE)
  {
    /* case Noise Error flag is raised : ... */
			LL_USART_ClearFlag_ORE(USART3);
			LL_USART_DeInit(USART3);
			MX_USART3_UART_Init();
			LL_USART_EnableIT_RXNE(USART3);
			LL_USART_EnableIT_ERROR(USART3);
			RxCounter = 0x00;
  }
	else if (isr_reg & LL_USART_SR_PE)
  {
    /* case Noise Error flag is raised : ... */
			LL_USART_ClearFlag_PE(USART3);
			LL_USART_DeInit(USART3);
			MX_USART3_UART_Init();
			LL_USART_EnableIT_RXNE(USART3);
			LL_USART_EnableIT_ERROR(USART3);
			RxCounter = 0x00;
  }
  else
  {
    /* Unexpected IT source : Set LED to Blinking mode to indicate error occurs */
			LL_USART_DeInit(USART3);
			MX_USART3_UART_Init();
			LL_USART_EnableIT_RXNE(USART3);
			LL_USART_EnableIT_ERROR(USART3);
			RxCounter = 0x00;
  }
}

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
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
