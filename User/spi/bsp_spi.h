/**
  ******************************************************************************
  * @file    bsp_spi.h
  * @author  phoenix
  * @version V1.0.0
  * @date    20-October-2017
  * @brief   This file provides set of firmware functions to manage Leds ,
  *          push-button and spi available on STM32F4-Discovery Kit from STMicroelectronics.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */ 

#ifndef _BSP_SPI_H_
#define _BSP_SPI_H_

#include "stm32f4xx_conf.h"
#include "./cc1101/cc1101.h"

/**
  * @brief  CC1101 SPI Interface pins
  */
#define CC1101_SPI                      SPI2
#define CC1101_SPI_CLK                  RCC_APB1Periph_SPI2

#define CC1101_SPI_SCK_PIN              GPIO_Pin_13                 /* PB.13 */
#define CC1101_SPI_SCK_GPIO_PORT        GPIOB                       /* GPIOB */
#define CC1101_SPI_SCK_GPIO_CLK         RCC_AHB1Periph_GPIOB
#define CC1101_SPI_SCK_SOURCE           GPIO_PinSource13
#define CC1101_SPI_SCK_AF               GPIO_AF_SPI2

#define CC1101_SPI_MISO_PIN             GPIO_Pin_14                 /* PB.14 */
#define CC1101_SPI_MISO_GPIO_PORT       GPIOB                       /* GPIOB */
#define CC1101_SPI_MISO_GPIO_CLK        RCC_AHB1Periph_GPIOB
#define CC1101_SPI_MISO_SOURCE          GPIO_PinSource14
#define CC1101_SPI_MISO_AF              GPIO_AF_SPI2

#define CC1101_SPI_MOSI_PIN             GPIO_Pin_15                 /* PB.15 */
#define CC1101_SPI_MOSI_GPIO_PORT       GPIOB                       /* GPIOB */
#define CC1101_SPI_MOSI_GPIO_CLK        RCC_AHB1Periph_GPIOB
#define CC1101_SPI_MOSI_SOURCE          GPIO_PinSource15
#define CC1101_SPI_MOSI_AF              GPIO_AF_SPI2

/*===========================================================================
------------------------------Internal IMPORT functions----------------------
you must offer the following functions for this module
1. uint8_t SPI_ExchangeByte(uint8_t input); // SPI Send and Receive function
2. CC1101_CSN_LOW();                        // Pull down the CSN line
3. CC1101_CSN_HIGH();                       // Pull up the CSN Line
===========================================================================*/
// CC1101相关控制引脚定义， CSN(PE2), IRQ(PE4), GDO2(PE6) 
#define CC1101_SPI_CSN_PIN              GPIO_Pin_2                  /* PE.02 */
#define CC1101_SPI_CSN_GPIO_PORT        GPIOE                       /* GPIOE */
#define CC1101_SPI_CSN_GPIO_CLK         RCC_AHB1Periph_GPIOE

#define CC1101_IRQ_PIN                  GPIO_Pin_4                  /* PE.04 */
#define CC1101_IRQ_GPIO_PORT            GPIOE                       /* GPIOE */
#define CC1101_IRQ_GPIO_CLK             RCC_AHB1Periph_GPIOE

#define CC1101_GDO2_PIN                 GPIO_Pin_6                  /* PE.06 */
#define CC1101_GDO2_GPIO_PORT           GPIOE                       /* GPIOE */
#define CC1101_GDO2_GPIO_CLK            RCC_AHB1Periph_GPIOE
#define CC1101_GDO2_EXTI_PORTSOURCE  		EXTI_PortSourceGPIOE
#define CC1101_GDO2_EXTI_PINSOURCE    	EXTI_PinSource6
#define CC1101_GDO2_EXTI_LINE         	EXTI_Line6
#define CC1101_GDO2_EXTI_IRQ          	EXTI9_5_IRQn
#define CC1101_GDO2_IRQHandler         	EXTI9_5_IRQHandler

#define CC1101_CSN_LOW()                GPIO_ResetBits(CC1101_SPI_CSN_GPIO_PORT,CC1101_SPI_CSN_PIN)

#define CC1101_CSN_HIGH()               GPIO_SetBits(CC1101_SPI_CSN_GPIO_PORT,CC1101_SPI_CSN_PIN)

#define CC1101_IRQ_READ()               GPIO_ReadInputDataBit(CC1101_IRQ_GPIO_PORT,CC1101_IRQ_PIN)

#define CC1101_GDO2_READ()             	GPIO_ReadInputDataBit(CC1101_GDO2_GPIO_PORT,CC1101_GDO2_PIN)

/**
  * @brief  LED Interface pins
  */
#define LED3_Orange_PIN     GPIO_Pin_13
#define LED4_Green_PIN      GPIO_Pin_12
#define LED5_Red_PIN        GPIO_Pin_14
#define	LED6_Blue_PIN       GPIO_Pin_15
#define	LED_GPIO_PORT       GPIOD
#define	LED_GPIO_CLK        RCC_AHB1Periph_GPIOD

// LED操作函数，(ON)打开, (OFF)关闭，(TOG)翻转
#define LED3_Orange_OFF()   GPIO_ResetBits(LED_GPIO_PORT,LED3_Orange_PIN)        
#define LED3_Orange_ON()    GPIO_SetBits(LED_GPIO_PORT,LED3_Orange_PIN)
#define LED3_Orange_TOG()   GPIO_ToggleBits(LED_GPIO_PORT,LED3_Orange_PIN)

#define LED4_Green_OFF()    GPIO_ResetBits(LED_GPIO_PORT,LED4_Green_PIN)        
#define LED4_Green_ON()     GPIO_SetBits(LED_GPIO_PORT,LED4_Green_PIN)
#define LED4_Green_TOG()    GPIO_ToggleBits(LED_GPIO_PORT,LED4_Green_PIN)

#define LED5_Red_OFF()      GPIO_ResetBits(LED_GPIO_PORT,LED5_Red_PIN)        
#define LED5_Red_ON()       GPIO_SetBits(LED_GPIO_PORT,LED5_Red_PIN)
#define LED5_Red_TOG()      GPIO_ToggleBits(LED_GPIO_PORT,LED5_Red_PIN)

#define LED6_Blue_OFF()     GPIO_ResetBits(LED_GPIO_PORT,LED6_Blue_PIN)        
#define LED6_Blue_ON()      GPIO_SetBits(LED_GPIO_PORT,LED6_Blue_PIN)
#define LED6_Blue_TOG()     GPIO_ToggleBits(LED_GPIO_PORT,LED6_Blue_PIN)

void GPIO_Initial(void);                // 初始化通用IO端口
void SPI_Initial(void);                 // 初始化SPI

uint8_t SPI_ExchangeByte(SPI_TypeDef* SPIx,uint8_t input);  // 通过SPI进行数据交换 

#endif //_BSP_SPI_H_

/******************* END OF FILE ******************/
