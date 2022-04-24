/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"

#include "stm32l0xx_ll_dma.h"
#include "stm32l0xx_ll_lpuart.h"
#include "stm32l0xx_ll_rcc.h"
#include "stm32l0xx_ll_spi.h"
#include "stm32l0xx_ll_usart.h"
#include "stm32l0xx_ll_system.h"
#include "stm32l0xx_ll_gpio.h"
#include "stm32l0xx_ll_exti.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_cortex.h"
#include "stm32l0xx_ll_utils.h"
#include "stm32l0xx_ll_pwr.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern __IO uint32_t CollectorID;
#define	_DEBUG    1 //  use printf debug

#if (_DEBUG == 1)
#define debug_printf(...)     			printf(__VA_ARGS__)
#define debug_info(format,...)   		printf("[info] %s() %d "format"\r\n",__func__,__LINE__,##__VA_ARGS__)
#define debug_error(format,...)  		printf("[error] %s %s()%d "format"\r\n",__FILE__,__func__,__LINE__,##__VA_ARGS__)

#else
#define debug_printf(...)     			{};
#define debug_info(format,...)     	{};
#define debug_error(format,...)    	{};
#endif
	
#define	_Error_Handler(__FILE__, __LINE__)	printf("[error] %s %d \r\n",__FILE__,__LINE__)

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define EEPROM_START_ADDR   0x08080000   /* Start @ of user eeprom area */
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TX_EN_Pin LL_GPIO_PIN_1
#define TX_EN_GPIO_Port GPIOA
#define SPI1_CS_Pin LL_GPIO_PIN_2
#define SPI1_CS_GPIO_Port GPIOA
#define GDO0_Pin LL_GPIO_PIN_3
#define GDO0_GPIO_Port GPIOA
#define GDO0_EXTI_IRQn EXTI2_3_IRQn
#define GDO2_Pin LL_GPIO_PIN_4
#define GDO2_GPIO_Port GPIOA
#define GDO2_EXTI_IRQn EXTI4_15_IRQn
#define RX_EN_Pin LL_GPIO_PIN_2
#define RX_EN_GPIO_Port GPIOB
#define LED1_Pin LL_GPIO_PIN_8
#define LED1_GPIO_Port GPIOA
#define LED2_Pin LL_GPIO_PIN_11
#define LED2_GPIO_Port GPIOA
#define LED_GREEN_Pin LL_GPIO_PIN_12
#define LED_GREEN_GPIO_Port GPIOA
#define USR4G_POWER_KEY_Pin LL_GPIO_PIN_3
#define USR4G_POWER_KEY_GPIO_Port GPIOB
#define USR4G_RESET_Pin LL_GPIO_PIN_4
#define USR4G_RESET_GPIO_Port GPIOB
#define USR4G_RELOAD_Pin LL_GPIO_PIN_5
#define USR4G_RELOAD_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
void SystemInitial(void);
void ShowMessage(void);
void ModuleLtePowerOn(void);
void strcatArray(uint8_t *dest, uint8_t *src, uint8_t position, uint8_t srclength);
void DATAEEPROM_Program(uint32_t Address, uint32_t Data);
uint32_t DATAEEPROM_Read(uint32_t Address);
void LED_Blinking(uint32_t Period);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
