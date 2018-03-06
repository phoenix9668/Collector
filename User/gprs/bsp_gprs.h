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
#define MOD_RESET_PIN     				GPIO_Pin_9
#define	MOD_RESET_PORT				GPIOA
#define MOD_RESET_CLK        	RCC_AHB1Periph_GPIOA

#define MOD_RELOAD_PIN      		GPIO_Pin_13
#define	MOD_RELOAD_PORT			GPIOD
#define MOD_RELOAD_CLK      	RCC_AHB1Periph_GPIOD

// ����������(ON)��, (OFF)�رգ�(TOG)��ת
#define MOD_RESET_OFF()   				GPIO_ResetBits(MOD_RESET_PORT,MOD_RESET_PIN)        
#define MOD_RESET_ON()    		GPIO_SetBits(MOD_RESET_PORT,MOD_RESET_PIN)
#define MOD_RESET_TOG()   		GPIO_ToggleBits(MOD_RESET_PORT,MOD_RESET_PIN)

#define MOD_RELOAD_OFF()    		GPIO_ResetBits(MOD_RELOAD_PORT,MOD_RELOAD_PIN)        
#define MOD_RELOAD_ON()   	GPIO_SetBits(MOD_RELOAD_PORT,MOD_RELOAD_PIN)
#define MOD_RELOAD_TOG()   	GPIO_ToggleBits(MOD_RELOAD_PORT,MOD_RELOAD_PIN)

void MOD_GPRS_Config(void); 

#endif //_BSP_SPI_H_

/******************* END OF FILE ******************/