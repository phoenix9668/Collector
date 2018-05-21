 /**
  ******************************************************************************
  * @file    bsp_COM1_usart.c
  * @author  phoenix
  * @version V1.0.0
  * @date    23-October-2017
  * @brief   重定向c库printf函数到usart端口，中断接收模式
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */ 
  
#include "./usart/bsp_com_usart.h"
#include "./gprs/bsp_gprs.h"

uint8_t aTxBuffer[BUFFERSIZE] = "USART DMA Example: Communication between two USART using DMA";
uint8_t aRxBuffer [BUFFERSIZE];

 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
void NVIC_Configuration(uint8_t NVIC_IRQChannel, uint8_t NVIC_IRQChannelPreemptionPriority, uint8_t NVIC_IRQChannelSubPriority)
{
    NVIC_InitTypeDef NVIC_InitStructure;
  
    /* 嵌套向量中断控制器组选择 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    /* 配置USART为中断源 */
    NVIC_InitStructure.NVIC_IRQChannel = NVIC_IRQChannel;
    /* 抢断优先级为1 */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_IRQChannelPreemptionPriority;
    /* 子优先级为1 */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_IRQChannelSubPriority;
    /* 使能中断 */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /* 初始化配置NVIC */
    NVIC_Init(&NVIC_InitStructure);
}

 /**
  * @brief  Configures the USART Peripheral.
  * @param  None
  * @retval None
  */
void COM1_USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* Peripheral Clock Enable -------------------------------------------------*/
  /* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(COM1_USART_RX_GPIO_CLK | COM1_USART_TX_GPIO_CLK,ENABLE);

	/* Enable USART clock */
	RCC_APB1PeriphClockCmd(COM1_USART_CLK, ENABLE);

  /* USARTx GPIO configuration -----------------------------------------------*/   
  /* Configure USART Tx and Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = COM1_USART_TX_PIN  ;  
	GPIO_Init(COM1_USART_TX_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = COM1_USART_RX_PIN;
	GPIO_Init(COM1_USART_RX_GPIO_PORT, &GPIO_InitStructure);
  
  /* Connect USART pins to AF */
	GPIO_PinAFConfig(COM1_USART_RX_GPIO_PORT, COM1_USART_RX_SOURCE, COM1_USART_RX_AF);
	GPIO_PinAFConfig(COM1_USART_TX_GPIO_PORT, COM1_USART_TX_SOURCE, COM1_USART_TX_AF);

  /* USARTx configuration ----------------------------------------------------*/
  /* USARTx configured as follows:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - one Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = COM1_USART_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	/* When using Parity the word length must be configured to 9 bits */
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(COM1_USART, &USART_InitStructure); 
	
	/* 使能串口接收中断 */
//	USART_ITConfig(COM1_USART, USART_IT_TC, DISABLE);	
//	USART_ITConfig(COM1_USART, USART_IT_RXNE, DISABLE);
//	USART_ITConfig(COM1_USART, USART_IT_TXE, DISABLE);
//	USART_ITConfig(COM1_USART, USART_IT_IDLE, ENABLE);
	
	/* 嵌套向量中断控制器NVIC配置 */
//	NVIC_Configuration(COM1_USART_IRQ,0,0);
	
  /* Enable USART DMA TX Requsts */
//	USART_DMACmd(COM1_USART,USART_DMAReq_Tx,ENABLE);  
  /* Enable USART DMA RX Requsts */
//	USART_DMACmd(COM1_USART,USART_DMAReq_Rx,ENABLE); 
	/* Enable USART */
	USART_Cmd(COM1_USART, ENABLE);
}

void COM1_DMA_Config(void)
{
	DMA_InitTypeDef  DMA_InitStructure;
	
	/* Enable the DMA clock */
  RCC_AHB1PeriphClockCmd(USART5_DMAx_CLK, ENABLE);
	DMA_DeInit(USART5_TX_DMA_STREAM);
	DMA_DeInit(USART5_RX_DMA_STREAM);
	
  /* Configure DMA controller to manage USART TX and RX DMA request ----------*/ 
  /* Configure DMA Initialization Structure */
  DMA_InitStructure.DMA_BufferSize = BUFFERSIZE ;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(COM1_USART->DR)) ;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  /* Configure TX DMA */
  DMA_InitStructure.DMA_Channel = USART5_TX_DMA_CHANNEL ;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;
  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)aTxBuffer ;
  DMA_Init(USART5_TX_DMA_STREAM,&DMA_InitStructure);
  /* Configure RX DMA */
  DMA_InitStructure.DMA_Channel = USART5_RX_DMA_CHANNEL ;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;
  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)aRxBuffer ; 
  DMA_Init(USART5_RX_DMA_STREAM,&DMA_InitStructure);
	
	DMA_ITConfig(USART5_TX_DMA_STREAM, DMA_IT_TC, ENABLE);
	/* Enable DMA USART TX Stream */
  DMA_Cmd(USART5_TX_DMA_STREAM,ENABLE);
	NVIC_Configuration(USART5_DMA_TX_IRQn,2,0);

//	DMA_ITConfig(USART5_RX_DMA_STREAM, DMA_IT_HT, ENABLE);
	/* Enable DMA USART RX Stream */
  DMA_Cmd(USART5_RX_DMA_STREAM,ENABLE);
	NVIC_Configuration(USART5_DMA_RX_IRQn,3,0);	
	
}

 /**
  * @brief  Configures the USART Peripheral.
  * @param  None
  * @retval None
  */
void COM2_USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* Peripheral Clock Enable -------------------------------------------------*/
  /* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(COM2_USART_RX_GPIO_CLK | COM2_USART_TX_GPIO_CLK,ENABLE);

	/* Enable USART clock */
	RCC_APB2PeriphClockCmd(COM2_USART_CLK, ENABLE);
  
  /* USARTx GPIO configuration -----------------------------------------------*/   
  /* Configure USART Tx and Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = COM2_USART_TX_PIN  ;  
	GPIO_Init(COM2_USART_TX_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = COM2_USART_RX_PIN;
	GPIO_Init(COM2_USART_RX_GPIO_PORT, &GPIO_InitStructure);
  
	/* Connect USART pins to AF */
	GPIO_PinAFConfig(COM2_USART_RX_GPIO_PORT, COM2_USART_RX_SOURCE, COM2_USART_RX_AF);
	GPIO_PinAFConfig(COM2_USART_TX_GPIO_PORT, COM2_USART_TX_SOURCE, COM2_USART_TX_AF);
  
  /* USARTx configuration ----------------------------------------------------*/
  /* USARTx configured as follows:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - one Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = COM2_USART_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	/* When using Parity the word length must be configured to 9 bits */
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(COM2_USART, &USART_InitStructure); 
	
	/* 嵌套向量中断控制器NVIC配置 */
//	NVIC_Configuration();
  
	/* 使能串口接收中断 */
//	USART_ITConfig(COM2_USART, USART_IT_RXNE, ENABLE);
	
	/* Enable USART */
	USART_Cmd(COM2_USART, ENABLE);
}

/*****************  发送一个字符 **********************/
void Usart_SendByte(USART_TypeDef *pUSARTx, uint8_t ch)
{
	/* 发送一个字节数据到USART */
	USART_SendData(pUSARTx, ch);
		
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/*****************  发送字符串 **********************/
void Usart_SendString(USART_TypeDef *pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte(pUSARTx, *(str + k));
      k++;
  } while(*(str+k) != '\0');
  
  /* 等待发送完成 */
  while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET)
  {}
}

/*****************  发送一个16位数 **********************/
void Usart_SendHalfWord(USART_TypeDef *pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	/* 取出高八位 */
	temp_h = (ch&0XFF00)>>8;
	/* 取出低八位 */
	temp_l = ch&0XFF;
	/* 发送高八位 */
	USART_SendData(pUSARTx, temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	/* 发送低八位 */
	USART_SendData(pUSARTx, temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
	/* 发送一个字节数据到串口 */
	USART_SendData(MOD_USART, (uint8_t)ch);
	/* 等待发送完毕 */
	while (USART_GetFlagStatus(MOD_USART, USART_FLAG_TXE) == RESET);		
	return (ch);
}

///重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
	/* 等待串口输入数据 */
	while (USART_GetFlagStatus(MOD_USART, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(MOD_USART);
}

//接收一个字节
uint8_t Usart_RecByte(USART_TypeDef *pUSARTx)
{
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_RXNE) == RESET);
	return (uint8_t)USART_ReceiveData(pUSARTx);
}

//接收两个字节
uint16_t Usart_RecHalfWord(USART_TypeDef *pUSARTx)
{
	uint8_t temp_h, temp_l;
	/* 接收第一个byte */
	temp_h = Usart_RecByte(pUSARTx);
	/* 接收第二个byte */
	temp_l = Usart_RecByte(pUSARTx);
	return ((uint16_t)(0xFF00 & temp_h<<8)+(uint16_t)(0x00FF & temp_l));
}

//接收四个字节
uint32_t Usart_RecWord(USART_TypeDef *pUSARTx)
{
	uint16_t temp_h, temp_l;
	/* 接收第一个HalfWord */
	temp_h = Usart_RecHalfWord(pUSARTx);
	/* 接收第二个HalfWord */
	temp_l = Usart_RecHalfWord(pUSARTx);
	return ((uint32_t)(0xFFFF0000 & temp_h<<16)+(uint32_t)(0x0000FFFF & temp_l));
}

//接收数组
void Usart_RecArray(USART_TypeDef *pUSARTx, uint8_t *arr)
{
	unsigned int k;
  for(k=0;k<PCCOMMAND_LENGTH;k++)
  {
			*(arr+k) = Usart_RecByte(pUSARTx);
			if(k>1)
			{
				if(*(arr+k-1) == 0x0d && *(arr+k) == 0x0a)
				{break;}
			}
  }
}
/*********************************************END OF FILE**********************/
