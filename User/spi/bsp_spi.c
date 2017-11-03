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

#include "./spi/bsp_spi.h"
#include "./usart/bsp_debug_usart.h"

/**
  * @brief  GPIO_Initial function
  * @param  None
  * @retval None
  */
void GPIO_Initial(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //开启GPIO外设时钟
    RCC_AHB1PeriphClockCmd(LED_GPIO_CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(CC1101_IRQ_GPIO_CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(CC1101_GDO2_GPIO_CLK, ENABLE);
    
    //配置LED3的GPIO引脚
    GPIO_InitStructure.GPIO_Pin = LED3_Orange_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
    
    //配置LED4的GPIO引脚
    GPIO_InitStructure.GPIO_Pin = LED4_Green_PIN;
    GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);

    //配置LED5的GPIO引脚
    GPIO_InitStructure.GPIO_Pin = LED5_Red_PIN;
    GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
    
    //配置LED6的GPIO引脚
    GPIO_InitStructure.GPIO_Pin = LED6_Blue_PIN;
    GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
    
    //配置IRQ的GPIO引脚
    GPIO_InitStructure.GPIO_Pin = CC1101_IRQ_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CC1101_IRQ_GPIO_PORT, &GPIO_InitStructure);
    
    //配置GDO2的GPIO引脚
    GPIO_InitStructure.GPIO_Pin = CC1101_GDO2_PIN;
    GPIO_Init(CC1101_GDO2_GPIO_PORT, &GPIO_InitStructure);
    
}

/**
  * @brief  SPI_Initial function
  * @param  None
  * @retval None
  */
void SPI_Initial(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    /* Enable the SPI periph */
    RCC_APB1PeriphClockCmd(CC1101_SPI_CLK,ENABLE);
    
    /* Enable SCK, MOSI and MISO GPIO clocks */
    RCC_AHB1PeriphClockCmd(CC1101_SPI_SCK_GPIO_CLK | CC1101_SPI_MISO_GPIO_CLK | CC1101_SPI_MOSI_GPIO_CLK | CC1101_SPI_CSN_GPIO_CLK, ENABLE);

    /* Set Pin AF */
    GPIO_PinAFConfig(CC1101_SPI_SCK_GPIO_PORT, CC1101_SPI_SCK_SOURCE, CC1101_SPI_SCK_AF);
	GPIO_PinAFConfig(CC1101_SPI_MISO_GPIO_PORT, CC1101_SPI_MISO_SOURCE, CC1101_SPI_MISO_AF); 
	GPIO_PinAFConfig(CC1101_SPI_MOSI_GPIO_PORT, CC1101_SPI_MOSI_SOURCE, CC1101_SPI_MOSI_AF); 
    
    /* Set SPI_SCK Pin */
    GPIO_InitStructure.GPIO_Pin = CC1101_SPI_SCK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CC1101_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
    
    /* Set SPI_MISO Pin */
    GPIO_InitStructure.GPIO_Pin = CC1101_SPI_MISO_PIN;
    GPIO_Init(CC1101_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
    
    /* Set SPI_MOSI Pin */
    GPIO_InitStructure.GPIO_Pin = CC1101_SPI_MOSI_PIN;
    GPIO_Init(CC1101_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);
    
    /* Set SPI_CSN Pin */
    GPIO_InitStructure.GPIO_Pin = CC1101_SPI_CSN_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(CC1101_SPI_CSN_GPIO_PORT, &GPIO_InitStructure);
    
    /* Set SPI_CSN Pin High */
    CC1101_CSN_HIGH();
    
    /* SPI configuration -------------------------------------------------------*/
    SPI_I2S_DeInit(CC1101_SPI);
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;//84MHz/32 = 3.625MHz
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_Init(CC1101_SPI, &SPI_InitStructure);

    /* Enable SPI1  */
    SPI_Cmd(CC1101_SPI, ENABLE);    

}

/*===========================================================================
* 函数 ：SPI_ExchangeByte() => 通过SPI进行数据交换                          * 
* 输入 ：需要写入SPI的值                                                    * 
* 输出 ：通过SPI读出的值                                                    * 
============================================================================*/
uint8_t SPI_ExchangeByte(SPI_TypeDef* SPIx,uint8_t input)
{
    SPI_SendData(SPIx,input);
    //printf("spi send data:%x\n",input);
	while (RESET == SPI_GetFlagStatus(SPIx,SPI_FLAG_TXE));   // 等待数据传输完成	
	while (RESET == SPI_GetFlagStatus(SPIx,SPI_FLAG_RXNE)){}; // 等待数据接收完成
    //printf("spi receive data:%x\n",SPI_ReceiveData(SPIx));
	return (SPI_ReceiveData(SPIx));
}

/******************* END OF FILE ******************/
