/**
  ******************************************************************************
  * @file    bsp_gprs.h
  * @author  phoenix
  * @version V1.0.0
  * @date    31-January-2017
  * @brief   This file provides set of firmware functions to manage Leds ,
  *          push-button and spi available on STM32F4-Discovery Kit from STMicroelectronics.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */ 

#ifndef __BSP_GPRS_H_
#define __BSP_GPRS_H_

#include "stm32f4xx.h"

/**
  * @brief  USR-GM3P Interface pins
  */
#define MOD_RESET_PIN     		GPIO_Pin_9
#define	MOD_RESET_PORT				GPIOA
#define MOD_RESET_CLK        	RCC_AHB1Periph_GPIOA

#define MOD_GPRS_PIN      		GPIO_Pin_10
#define	MOD_GPRS_PORT					GPIOA
#define MOD_GPRS_CLK      		RCC_AHB1Periph_GPIOA

// 操作函数，(ON)打开, (OFF)关闭，(TOG)翻转
#define MOD_RESET_OFF()   		GPIO_ResetBits(MOD_RESET_PORT,MOD_RESET_PIN)        
#define MOD_RESET_ON()    		GPIO_SetBits(MOD_RESET_PORT,MOD_RESET_PIN)
#define MOD_RESET_TOG()   		GPIO_ToggleBits(MOD_RESET_PORT,MOD_RESET_PIN)

#define MOD_GPRS_READ()  			GPIO_ReadInputDataBit(MOD_GPRS_PORT,MOD_GPRS_PIN)

//串口定义
/*******************************************************/
#define MOD_USART                             	USART3
#define MOD_USART_CLK                         	RCC_APB1Periph_USART3
#define MOD_USART_BAUDRATE                    	115200  //串口波特率

#define MOD_USART_RX_GPIO_PORT                	GPIOB
#define MOD_USART_RX_GPIO_CLK                 	RCC_AHB1Periph_GPIOB
#define MOD_USART_RX_PIN                      	GPIO_Pin_11
#define MOD_USART_RX_AF                       	GPIO_AF_USART3
#define MOD_USART_RX_SOURCE                   	GPIO_PinSource11

#define MOD_USART_TX_GPIO_PORT                	GPIOB
#define MOD_USART_TX_GPIO_CLK                 	RCC_AHB1Periph_GPIOB
#define MOD_USART_TX_PIN                      	GPIO_Pin_10
#define MOD_USART_TX_AF                       	GPIO_AF_USART3
#define MOD_USART_TX_SOURCE                   	GPIO_PinSource10

#define MOD_USART_IRQHandler                  	USART3_IRQHandler
#define MOD_USART_IRQ                 	    		USART3_IRQn
/************************************************************/

void MOD_GPRS_Config(void); 
void MOD_USART_Config(void);

#endif //__BSP_GPRS_H_

/******************* END OF FILE ******************/
