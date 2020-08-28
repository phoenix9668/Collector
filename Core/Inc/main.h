/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_dma.h"

#include "stm32f4xx_ll_exti.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define RXBUFFERSIZE  		21			// Size of Reception buffer
#define ACK_SLENGTH   		27   		// 反馈数据包长度
#define ACK_LLENGTH   		69   		// 反馈数据包长度27+41+1
#define SEND_LENGTH     	18			// 发送数据包长度
#define RECV_LENGTH   		61			// 接收数据包长度19+41+1
#define SEND_PACKAGE_NUM	200			// 发生数据包数
#define RECV_TIMEOUT			30000000  		// 接收等待13s
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_COM_Pin GPIO_PIN_2
#define LED_COM_GPIO_Port GPIOE
#define LED_STA_Pin GPIO_PIN_3
#define LED_STA_GPIO_Port GPIOE
#define LED_RUN_Pin GPIO_PIN_4
#define LED_RUN_GPIO_Port GPIOE
#define SPI2_CS1_Pin GPIO_PIN_2
#define SPI2_CS1_GPIO_Port GPIOB
#define SPI2_CS2_Pin GPIO_PIN_12
#define SPI2_CS2_GPIO_Port GPIOB
#define MOD_RESET_Pin GPIO_PIN_9
#define MOD_RESET_GPIO_Port GPIOA
#define MOD_GPRS_Pin GPIO_PIN_10
#define MOD_GPRS_GPIO_Port GPIOA
#define GDO0_Pin GPIO_PIN_0
#define GDO0_GPIO_Port GPIOE
#define GDO0_EXTI_IRQn EXTI0_IRQn
#define GDO2_Pin GPIO_PIN_1
#define GDO2_GPIO_Port GPIOE
#define GDO2_EXTI_IRQn EXTI1_IRQn
/* USER CODE BEGIN Private defines */
void Show_Message(void);
void Function_Ctrl(uint8_t *command);
void Check_All_RFID(uint8_t *command);
void Cfg_Assign_RFID(uint8_t *command);
void Check_Assign_RFID(uint8_t *command);
void Check_Assign_Section_RFID(uint8_t *command);
void Prog_Assign_RFID(uint8_t *command);
void Clear_Assign_RFID(uint8_t *command);
void Clear_Assign_Section_RFID(uint8_t *command);
void RF_ProgPacket(uint8_t *command);
void RF_SendPacket(uint8_t *command);
void RF_SendClearPacket(uint8_t *command);
uint8_t	RF_Acknowledge(void);
void Reply_PC(uint8_t index);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
