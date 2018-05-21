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
#include "./usart/bsp_com_usart.h"

/**
  * @brief  EXTI_Config function
  * @param  None
  * @retval None
  */
void EXTI_Config(uint32_t EXTI_Line, EXTITrigger_TypeDef EXTI_Trigger, FunctionalState EXTI_LineCmd)
{
		EXTI_InitTypeDef EXTI_InitStructure;
		/* Configure EXTI Line */
		EXTI_InitStructure.EXTI_Line = EXTI_Line;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger;  
		EXTI_InitStructure.EXTI_LineCmd = EXTI_LineCmd;
		EXTI_Init(&EXTI_InitStructure);
}

/**
  * @brief  NVIC_Config function
  * @param  None
  * @retval None
  */
void NVIC_Config(uint8_t NVIC_IRQChannel, uint8_t NVIC_IRQChannelSubPriority)
{
		NVIC_InitTypeDef NVIC_InitStructure;
		/* Enable and set EXTI Line Interrupt to the lowest priority */
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
		NVIC_InitStructure.NVIC_IRQChannel = NVIC_IRQChannel;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_IRQChannelSubPriority;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  GPIO_Initial function
  * @param  None
  * @retval None
  */
void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
    /* Enable GPIOE clock */
    RCC_AHB1PeriphClockCmd(LED_GPIO_CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(CC1101_IRQ_GPIO_CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(CC1101_GDO2_GPIO_CLK, ENABLE);
    
		//使能 SYSCFG 时钟 ,使用GPIO外部中断时必须使能SYSCFG时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);  
	
    //配置LED3的GPIO引脚
    GPIO_InitStructure.GPIO_Pin = LED_RUN_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
    
    //配置LED4的GPIO引脚
    GPIO_InitStructure.GPIO_Pin = LED_STA_PIN;
    GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);

    //配置LED5的GPIO引脚
    GPIO_InitStructure.GPIO_Pin = LED_COM_PIN;
    GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
    
    //配置IRQ的GPIO引脚
    GPIO_InitStructure.GPIO_Pin = CC1101_IRQ_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CC1101_IRQ_GPIO_PORT, &GPIO_InitStructure);
		
		/* Connect EXTI Line0 to PE0 pin */
		SYSCFG_EXTILineConfig(CC1101_IRQ_EXTI_PORTSOURCE,CC1101_IRQ_EXTI_PINSOURCE);

		/* Configure EXTI Line0 */
		EXTI_Config(CC1101_IRQ_EXTI_LINE, EXTI_Trigger_Rising, ENABLE);
		
		/* Enable and set EXTI Line0 Interrupt to the lowest priority */
		NVIC_Config(CC1101_IRQ_EXTI_IRQ, 1);
    
    //配置GDO2的GPIO引脚
    GPIO_InitStructure.GPIO_Pin = CC1101_GDO2_PIN;
    GPIO_Init(CC1101_GDO2_GPIO_PORT, &GPIO_InitStructure);

		/* Connect EXTI Line1 to PE1 pin */
		SYSCFG_EXTILineConfig(CC1101_GDO2_EXTI_PORTSOURCE,CC1101_GDO2_EXTI_PINSOURCE);

		/* Configure EXTI Line1 */
		EXTI_Config(CC1101_GDO2_EXTI_LINE, EXTI_Trigger_Rising, ENABLE);

		/* Enable and set EXTI Line1 Interrupt to the lowest priority */
		NVIC_Config(CC1101_GDO2_EXTI_IRQ, 2);
		
		LED_RUN_OFF();
		LED_STA_OFF();
		LED_COM_OFF();
}

/**
  * @brief  SPI_Initial function
  * @param  None
  * @retval None
  */
void SPI_Config(void)
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
		
		/* Set FRAM_SPI_CSN Pin */
    GPIO_InitStructure.GPIO_Pin = FRAM_SPI_CSN_PIN;
    GPIO_Init(CC1101_SPI_CSN_GPIO_PORT, &GPIO_InitStructure);
    
    /* Set FRAM_SPI_CSN Pin High */
    FRAM_CSN_HIGH();
    
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
//    printf("spi send data:%x\n",input);
	while (RESET == SPI_GetFlagStatus(SPIx,SPI_FLAG_TXE));   // 等待数据传输完成	
	while (RESET == SPI_GetFlagStatus(SPIx,SPI_FLAG_RXNE)){}; // 等待数据接收完成
//    printf("spi receive data:%x\n",SPI_ReceiveData(SPIx));
	return (SPI_ReceiveData(SPIx));
}

// Function for sending and receiving data through SPI
void SpiFunction(SPI_TypeDef* SPIx, uint8_t OutputBuff[], uint8_t InputBuff[], uint16_t OutNoOfBytes, uint16_t InNoOfBytes)
{
	uint16_t i;

	for(i=0;i<OutNoOfBytes;i++)
	{
    SPI_ExchangeByte(SPIx, OutputBuff[i]);					// Send data
	}
   
	for(i=0;i<InNoOfBytes;i++)
	{
		InputBuff[i] = SPI_ExchangeByte(SPIx, 0xFF);		// Receive data
	}
}

/******************* END OF FILE ******************/
