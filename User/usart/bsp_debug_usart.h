#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H

#include "stm32f4xx.h"
#include <stdio.h>


//引脚定义
/*******************************************************/
#define DEBUG_USART                             USART3
#define DEBUG_USART_CLK                         RCC_APB1Periph_USART3
#define DEBUG_USART_BAUDRATE                    115200  //串口波特率

#define DEBUG_USART_RX_GPIO_PORT                GPIOD
#define DEBUG_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOD
#define DEBUG_USART_RX_PIN                      GPIO_Pin_9
#define DEBUG_USART_RX_AF                       GPIO_AF_USART3
#define DEBUG_USART_RX_SOURCE                   GPIO_PinSource9

#define DEBUG_USART_TX_GPIO_PORT                GPIOD
#define DEBUG_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOD
#define DEBUG_USART_TX_PIN                      GPIO_Pin_8
#define DEBUG_USART_TX_AF                       GPIO_AF_USART3
#define DEBUG_USART_TX_SOURCE                   GPIO_PinSource8

#define DEBUG_USART_IRQHandler                  USART3_IRQHandler
#define DEBUG_USART_IRQ                 	    	USART3_IRQn
/************************************************************/

void Debug_USART_Config(void);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);

void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);

#endif /* __USART1_H */
