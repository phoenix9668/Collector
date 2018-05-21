#ifndef __COM_USART_H
#define	__COM_USART_H

#include "stm32f4xx.h"
#include <stdio.h>

#define	UART_DEBUG
#define PCCOMMAND_LENGTH		20
#define BUFFERSIZE	100	/* Transmit buffer size */

//COM1引脚定义
/*******************************************************/
#define COM1_USART                      	UART5
#define COM1_USART_CLK                  	RCC_APB1Periph_UART5
#define COM1_USART_BAUDRATE             	115200  //串口波特率

#define COM1_USART_RX_GPIO_PORT         	GPIOD
#define COM1_USART_RX_GPIO_CLK        		RCC_AHB1Periph_GPIOD
#define COM1_USART_RX_PIN               	GPIO_Pin_2
#define COM1_USART_RX_AF                 	GPIO_AF_UART5
#define COM1_USART_RX_SOURCE            	GPIO_PinSource2

#define COM1_USART_TX_GPIO_PORT          	GPIOC
#define COM1_USART_TX_GPIO_CLK        		RCC_AHB1Periph_GPIOC
#define COM1_USART_TX_PIN              		GPIO_Pin_12
#define COM1_USART_TX_AF                	GPIO_AF_UART5
#define COM1_USART_TX_SOURCE             	GPIO_PinSource12

#define COM1_USART_IRQHandler           	UART5_IRQHandler
#define COM1_USART_IRQ                 		UART5_IRQn
/************************************************************/

//COM2引脚定义
/*******************************************************/
#define COM2_USART                      	USART6
#define COM2_USART_CLK                 		RCC_APB2Periph_USART6
#define COM2_USART_BAUDRATE             	115200  //串口波特率

#define COM2_USART_RX_GPIO_PORT         	GPIOC
#define COM2_USART_RX_GPIO_CLK           	RCC_AHB1Periph_GPIOC
#define COM2_USART_RX_PIN                	GPIO_Pin_7
#define COM2_USART_RX_AF                 	GPIO_AF_USART6
#define COM2_USART_RX_SOURCE           		GPIO_PinSource7

#define COM2_USART_TX_GPIO_PORT        		GPIOC
#define COM2_USART_TX_GPIO_CLK           	RCC_AHB1Periph_GPIOC
#define COM2_USART_TX_PIN               	GPIO_Pin_6
#define COM2_USART_TX_AF                 	GPIO_AF_USART6
#define COM2_USART_TX_SOURCE            	GPIO_PinSource6

#define COM2_USART_IRQHandler           	USART6_IRQHandler
#define COM2_USART_IRQ                 		USART6_IRQn
/************************************************************/

/* Definition for DMA1 resources ********************************************/
#define USART5_DR_ADDRESS                ((uint32_t)UART5 + 0x04) 

#define USART5_DMA                       DMA1
#define USART5_DMAx_CLK                  RCC_AHB1Periph_DMA1

#define USART5_TX_DMA_CHANNEL            DMA_Channel_4
#define USART5_TX_DMA_STREAM             DMA1_Stream7
#define USART5_TX_DMA_IT_FEIF 					 DMA_IT_FEIF7
#define USART5_TX_DMA_IT_DMEIF         	 DMA_IT_DMEIF7
#define USART5_TX_DMA_IT_TEIF          	 DMA_IT_TEIF7
#define USART5_TX_DMA_IT_HTIF         	 DMA_IT_HTIF7
#define USART5_TX_DMA_IT_TCIF         	 DMA_IT_TCIF7
              
#define USART5_RX_DMA_CHANNEL            DMA_Channel_4
#define USART5_RX_DMA_STREAM             DMA1_Stream0
#define USART5_RX_DMA_IT_FEIF         	 DMA_IT_FEIF0
#define USART5_RX_DMA_IT_DMEIF        	 DMA_IT_DMEIF0
#define USART5_RX_DMA_IT_TEIF        	   DMA_IT_TEIF0
#define USART5_RX_DMA_IT_HTIF         	 DMA_IT_HTIF0
#define USART5_RX_DMA_IT_TCIF         	 DMA_IT_TCIF0

#define USART5_DMA_TX_IRQn               DMA1_Stream7_IRQn
#define USART5_DMA_RX_IRQn               DMA1_Stream0_IRQn
#define USART5_DMA_TX_IRQHandler         DMA1_Stream7_IRQHandler
#define USART5_DMA_RX_IRQHandler         DMA1_Stream0_IRQHandler
/************************************************************/

void NVIC_Configuration(uint8_t NVIC_IRQChannel, uint8_t NVIC_IRQChannelPreemptionPriority, uint8_t NVIC_IRQChannelSubPriority);
void COM1_USART_Config(void);
void COM2_USART_Config(void);
void COM1_DMA_Config(void);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);

void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);
uint8_t Usart_RecByte(USART_TypeDef *pUSARTx);
uint16_t Usart_RecHalfWord(USART_TypeDef *pUSARTx);
uint32_t Usart_RecWord(USART_TypeDef *pUSARTx);
void Usart_RecArray(USART_TypeDef *pUSARTx, uint8_t *arr);

#endif /* __COM_H */
