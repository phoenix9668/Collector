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

#ifndef __SPI_H_
#define __SPI_H_

#include "stm32f4xx_conf.h"
#include "wizchip_conf.h"

/**
  * @brief  W5500 SPI Interface pins
  */
#define W5500_SPI                      	SPI1
#define W5500_SPI_CLK                  	RCC_APB2Periph_SPI1

#define W5500_SPI_SCK_PIN              	GPIO_Pin_5                 	/* PA.05 */
#define W5500_SPI_SCK_GPIO_PORT        	GPIOA                       /* GPIOA */
#define W5500_SPI_SCK_GPIO_CLK         	RCC_AHB1Periph_GPIOA
#define W5500_SPI_SCK_SOURCE           	GPIO_PinSource5
#define W5500_SPI_SCK_AF               	GPIO_AF_SPI1

#define W5500_SPI_MISO_PIN             	GPIO_Pin_6                 	/* PA.06 */
#define W5500_SPI_MISO_GPIO_PORT       	GPIOA                       /* GPIOA */
#define W5500_SPI_MISO_GPIO_CLK        	RCC_AHB1Periph_GPIOA
#define W5500_SPI_MISO_SOURCE          	GPIO_PinSource6
#define W5500_SPI_MISO_AF              	GPIO_AF_SPI1

#define W5500_SPI_MOSI_PIN             	GPIO_Pin_7                	/* PA.07 */
#define W5500_SPI_MOSI_GPIO_PORT       	GPIOA                       /* GPIOA */
#define W5500_SPI_MOSI_GPIO_CLK        	RCC_AHB1Periph_GPIOA
#define W5500_SPI_MOSI_SOURCE          	GPIO_PinSource7
#define W5500_SPI_MOSI_AF              	GPIO_AF_SPI1

#define W5500_SPI_CSN_PIN              	GPIO_Pin_14               	/* PE.14 */
#define W5500_SPI_CSN_GPIO_PORT        	GPIOE                       /* GPIOE */
#define W5500_SPI_CSN_GPIO_CLK         	RCC_AHB1Periph_GPIOE

void W5500_SPI_Config(void);
void W5500_SPI_cbfunc(void);
void SPI_WriteByte(uint8_t TxData);
uint8_t SPI_ReadByte(void);
void SPI_CrisEnter(void);
void SPI_CrisExit(void);
void SPI_CS_Select(void);
void SPI_CS_Deselect(void);

#endif //_SPI_H_

/******************* END OF FILE ******************/
