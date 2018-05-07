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

#include "spi.h"

/**
  * @brief  SPI_Initial function
  * @param  None
  * @retval None
  */
void W5500_SPI_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    /* Enable the SPI periph */
    RCC_APB2PeriphClockCmd(W5500_SPI_CLK,ENABLE);
    
    /* Enable SCK, MOSI and MISO GPIO clocks */
    RCC_AHB1PeriphClockCmd(W5500_SPI_SCK_GPIO_CLK | W5500_SPI_MISO_GPIO_CLK | W5500_SPI_MOSI_GPIO_CLK | W5500_SPI_CSN_GPIO_CLK, ENABLE);

    /* Set Pin AF */
    GPIO_PinAFConfig(W5500_SPI_SCK_GPIO_PORT, W5500_SPI_SCK_SOURCE, W5500_SPI_SCK_AF);
		GPIO_PinAFConfig(W5500_SPI_MISO_GPIO_PORT, W5500_SPI_MISO_SOURCE, W5500_SPI_MISO_AF); 
		GPIO_PinAFConfig(W5500_SPI_MOSI_GPIO_PORT, W5500_SPI_MOSI_SOURCE, W5500_SPI_MOSI_AF); 
    
    /* Set SPI_SCK Pin */
    GPIO_InitStructure.GPIO_Pin = W5500_SPI_SCK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(W5500_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
    
    /* Set SPI_MISO Pin */
    GPIO_InitStructure.GPIO_Pin = W5500_SPI_MISO_PIN;
    GPIO_Init(W5500_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
    
    /* Set SPI_MOSI Pin */
    GPIO_InitStructure.GPIO_Pin = W5500_SPI_MOSI_PIN;
    GPIO_Init(W5500_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);
    
    /* Set SPI_CSN Pin */
    GPIO_InitStructure.GPIO_Pin = W5500_SPI_CSN_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(W5500_SPI_CSN_GPIO_PORT, &GPIO_InitStructure);
    
    /* Set SPI_CSN Pin High */
    GPIO_SetBits(W5500_SPI_CSN_GPIO_PORT,W5500_SPI_CSN_PIN);
    
    /* SPI configuration -------------------------------------------------------*/
    SPI_I2S_DeInit(W5500_SPI);
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;//84MHz/32 = 3.625MHz
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_Init(W5500_SPI, &SPI_InitStructure);

    /* Enable SPI1  */
    SPI_Cmd(W5500_SPI, ENABLE);    

}

void W5500_SPI_cbfunc(void)
{
		// First of all, Should register SPI callback functions implemented by user for accessing WIZCHIP 
		/* Critical section callback */
		reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);   //注册临界区函数
    /* Chip selection call back */
		#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
			reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);//注册SPI片选信号函数
		#elif _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
			reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);  // CS must be tried with LOW.
		#else
		#if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SIP_) != _WIZCHIP_IO_MODE_SIP_
      #error "Unknown _WIZCHIP_IO_MODE_"
		#else
      reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
		#endif
		#endif
    /* SPI Read & Write callback function */
    reg_wizchip_spi_cbfunc(SPI_ReadByte, SPI_WriteByte);    //注册读写函数
}

/**
  * @brief 写1字节数据到SPI总线
  * @param TxData 写到总线的数据
  * @retval None
  */
void SPI_WriteByte(uint8_t TxData)
{                
    while((SPI1->SR&SPI_I2S_FLAG_TXE)==0);		//等待发送区空
    SPI1->DR=TxData;
    while((SPI1->SR&SPI_I2S_FLAG_RXNE)==0); 	//等待接收完1byte  
    SPI1->DR;        
}
/**
  * @brief 从SPI总线读取1字节数据
  * @retval 读到的数据
  */
uint8_t SPI_ReadByte(void)
{            
    while((SPI1->SR&SPI_I2S_FLAG_TXE)==0);		//等待发送区空           
    SPI1->DR=0xFF;                                               //???????????????? 
    while((SPI1->SR&SPI_I2S_FLAG_RXNE)==0);		//等待接收完1byte
    return SPI1->DR;
}
/**
  * @brief 进入临界区
  * @retval None
  */
void SPI_CrisEnter(void)
{
    __set_PRIMASK(1);
}
/**
  * @brief 退出临界区
  * @retval None
  */
void SPI_CrisExit(void)
{
    __set_PRIMASK(0);
}
 
/**
  * @brief 片选信号输出低电平
  * @retval None
  */
void SPI_CS_Select(void)
{
    GPIO_ResetBits(W5500_SPI_CSN_GPIO_PORT,W5500_SPI_CSN_PIN);
}
/**
  * @brief 片选信号输出高电平
  * @retval None
  */
void SPI_CS_Deselect(void)
{
    GPIO_SetBits(W5500_SPI_CSN_GPIO_PORT,W5500_SPI_CSN_PIN);
}

/******************* END OF FILE ******************/
