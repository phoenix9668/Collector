 /**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  phoenix
  * @version V1.0.0
  * @date    23-October-2017
  * @brief   重定向c库printf函数到usart端口，中断接收模式
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */ 
  
#include "./usart/bsp_debug_usart.h"


 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
//static void NVIC_Configuration(void)
//{
//    NVIC_InitTypeDef NVIC_InitStructure;
//  
//    /* 嵌套向量中断控制器组选择 */
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//  
//    /* 配置USART为中断源 */
//    NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;
//    /* 抢断优先级为1 */
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//    /* 子优先级为1 */
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//    /* 使能中断 */
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    /* 初始化配置NVIC */
//    NVIC_Init(&NVIC_InitStructure);
//}


 /**
  * @brief  DEBUG_USART GPIO 配置,工作模式配置。115200 8-N-1 ，中断接收模式
  * @param  无
  * @retval 无
  */
void Debug_USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_AHB1PeriphClockCmd(DEBUG_USART_RX_GPIO_CLK | DEBUG_USART_TX_GPIO_CLK,ENABLE);

	/* 使能 USART 时钟 */
	RCC_APB1PeriphClockCmd(DEBUG_USART_CLK, ENABLE);
  
	/* GPIO初始化 */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
	/* 配置Tx引脚为复用功能  */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_PIN  ;  
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

	/* 配置Rx引脚为复用功能 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_PIN;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
  
	/* 连接 PXx 到 USARTx_Tx*/
	GPIO_PinAFConfig(DEBUG_USART_RX_GPIO_PORT, DEBUG_USART_RX_SOURCE, DEBUG_USART_RX_AF);

	/* 连接 PXx 到 USARTx__Rx*/
	GPIO_PinAFConfig(DEBUG_USART_TX_GPIO_PORT, DEBUG_USART_TX_SOURCE, DEBUG_USART_TX_AF);
  
	/* 配置串DEBUG_USART 模式 */
	/* 波特率设置：DEBUG_USART_BAUDRATE */
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
	/* 字长(数据位+校验位)：8 */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	/* 停止位：1个停止位 */
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	/* 校验位选择：不使用校验 */
	USART_InitStructure.USART_Parity = USART_Parity_No;
	/* 硬件流控制：不使用硬件流 */
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	/* USART模式控制：同时使能接收和发送 */
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	/* 完成USART初始化配置 */
	USART_Init(DEBUG_USART, &USART_InitStructure); 
	
	/* 嵌套向量中断控制器NVIC配置 */
//	NVIC_Configuration();
  
	/* 使能串口接收中断 */
//	USART_ITConfig(DEBUG_USART, USART_IT_RXNE, ENABLE);
	
	/* 使能串口 */
	USART_Cmd(DEBUG_USART, ENABLE);
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
	USART_SendData(DEBUG_USART, (uint8_t)ch);
	/* 等待发送完毕 */
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);		
	return (ch);
}

///重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
	/* 等待串口输入数据 */
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(DEBUG_USART);
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
  for(k=0;k<PCCOMMEND_LENGTH;k++)
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
