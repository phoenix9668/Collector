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

#ifndef __BSP_SPI_H_
#define __BSP_SPI_H_

#include "stm32f4xx_conf.h"

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
// CC1101相关控制引脚定义， CSN(PB2), IRQ(PE0), GDO2(PE1) 
#define CC1101_SPI_CSN_PIN              GPIO_Pin_2                  /* PB.02 */
#define CC1101_SPI_CSN_GPIO_PORT        GPIOB                       /* GPIOB */
#define CC1101_SPI_CSN_GPIO_CLK         RCC_AHB1Periph_GPIOB

#define CC1101_IRQ_PIN                  GPIO_Pin_0                  /* PE.00 */
#define CC1101_IRQ_GPIO_PORT            GPIOE                       /* GPIOE */
#define CC1101_IRQ_GPIO_CLK             RCC_AHB1Periph_GPIOE
#define CC1101_IRQ_EXTI_PORTSOURCE  		EXTI_PortSourceGPIOE
#define CC1101_IRQ_EXTI_PINSOURCE    		EXTI_PinSource0
#define CC1101_IRQ_EXTI_LINE         		EXTI_Line0
#define CC1101_IRQ_EXTI_IRQ          		EXTI0_IRQn
#define CC1101_IRQ_IRQHandler         	EXTI0_IRQHandler

#define CC1101_GDO2_PIN                 GPIO_Pin_1                  /* PE.01 */
#define CC1101_GDO2_GPIO_PORT           GPIOE                       /* GPIOE */
#define CC1101_GDO2_GPIO_CLK            RCC_AHB1Periph_GPIOE
#define CC1101_GDO2_EXTI_PORTSOURCE  		EXTI_PortSourceGPIOE
#define CC1101_GDO2_EXTI_PINSOURCE    	EXTI_PinSource1
#define CC1101_GDO2_EXTI_LINE         	EXTI_Line1
#define CC1101_GDO2_EXTI_IRQ          	EXTI1_IRQn
#define CC1101_GDO2_IRQHandler         	EXTI1_IRQHandler

#define CC1101_CSN_LOW()                GPIO_ResetBits(CC1101_SPI_CSN_GPIO_PORT,CC1101_SPI_CSN_PIN)

#define CC1101_CSN_HIGH()               GPIO_SetBits(CC1101_SPI_CSN_GPIO_PORT,CC1101_SPI_CSN_PIN)

#define CC1101_IRQ_READ()               GPIO_ReadInputDataBit(CC1101_IRQ_GPIO_PORT,CC1101_IRQ_PIN)

#define CC1101_GDO2_READ()             	GPIO_ReadInputDataBit(CC1101_GDO2_GPIO_PORT,CC1101_GDO2_PIN)

/**
  * @brief  FM25L256 SPI Interface pins
  */
#define FRAM_SPI_CSN_PIN     						GPIO_Pin_12               	/* PB.12 */
#define FRAM_SPI_CSN_GPIO_PORT        	GPIOB                       /* GPIOB */
#define FRAM_SPI_CSN_GPIO_CLK         	RCC_AHB1Periph_GPIOB

#define FRAM_CSN_LOW()                	GPIO_ResetBits(FRAM_SPI_CSN_GPIO_PORT,FRAM_SPI_CSN_PIN)

#define FRAM_CSN_HIGH()               	GPIO_SetBits(FRAM_SPI_CSN_GPIO_PORT,FRAM_SPI_CSN_PIN)


/**
  * @brief  LED Interface pins
  */
#define LED_RUN_PIN     		GPIO_Pin_4
#define LED_STA_PIN      		GPIO_Pin_3
#define LED_COM_PIN        	GPIO_Pin_2
#define	LED_GPIO_PORT       GPIOE
#define	LED_GPIO_CLK        RCC_AHB1Periph_GPIOE

// LED操作函数，(ON)打开, (OFF)关闭，(TOG)翻转
#define LED_RUN_ON()   			GPIO_ResetBits(LED_GPIO_PORT,LED_RUN_PIN)        
#define LED_RUN_OFF()    		GPIO_SetBits(LED_GPIO_PORT,LED_RUN_PIN)
#define LED_RUN_TOG()   		GPIO_ToggleBits(LED_GPIO_PORT,LED_RUN_PIN)

#define LED_STA_ON()    		GPIO_ResetBits(LED_GPIO_PORT,LED_STA_PIN)        
#define LED_STA_OFF()     	GPIO_SetBits(LED_GPIO_PORT,LED_STA_PIN)
#define LED_STA_TOG()    		GPIO_ToggleBits(LED_GPIO_PORT,LED_STA_PIN)

#define LED_COM_ON()      	GPIO_ResetBits(LED_GPIO_PORT,LED_COM_PIN)        
#define LED_COM_OFF()      	GPIO_SetBits(LED_GPIO_PORT,LED_COM_PIN)
#define LED_COM_TOG()      	GPIO_ToggleBits(LED_GPIO_PORT,LED_COM_PIN)

void EXTI_Config(uint32_t EXTI_Line, EXTITrigger_TypeDef EXTI_Trigger, FunctionalState EXTI_LineCmd);
void NVIC_Config(uint8_t NVIC_IRQChannel, uint8_t NVIC_IRQChannelSubPriority);
void GPIO_Config(void);                // 初始化通用IO端口
void SPI_Config(void);                 // 初始化SPI

uint8_t SPI_ExchangeByte(SPI_TypeDef* SPIx,uint8_t input);  // 通过SPI进行数据交换 
void SpiFunction(SPI_TypeDef* SPIx, uint8_t OutputBuff[], uint8_t InputBuff[], uint16_t OutNoOfBytes, uint16_t InNoOfBytes);

#endif //_BSP_SPI_H_

/******************* END OF FILE ******************/
