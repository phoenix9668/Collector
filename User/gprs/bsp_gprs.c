/**
  ******************************************************************************
  * @file    bsp_gprs.c
  * @author  phoenix
  * @version V1.0.0
  * @date    31-January-2017
  * @brief   This file provides set of firmware functions to manage Leds ,
  *          push-button and spi available on STM32F4-Discovery Kit from STMicroelectronics.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */ 
	
#include "./gprs/bsp_gprs.h"
#include "./usart/bsp_com_usart.h"
extern void Delay(__IO uint32_t nCount);
/**
  * @brief  MOD_GPRS_Config function
  * @param  None
  * @retval None
  */
void MOD_GPRS_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
    //����GPIO����ʱ��
    RCC_AHB1PeriphClockCmd(MOD_RESET_CLK|MOD_GPRS_CLK, ENABLE);
	
    //����MOD_RESET��GPIO����
    GPIO_InitStructure.GPIO_Pin = MOD_RESET_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MOD_RESET_PORT, &GPIO_InitStructure);
    
    //����MOD_GPRS��GPIO����
    GPIO_InitStructure.GPIO_Pin = MOD_GPRS_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_Init(MOD_GPRS_PORT, &GPIO_InitStructure);
	
		MOD_RESET_OFF();
		Delay(0xFFFFFFF);
		MOD_RESET_ON();
		Delay(0x2FFFFFFF);
		printf("GPRS Module Reset Complete\n");
}

 /**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */
//static void NVIC_Configuration(void)
//{
//    NVIC_InitTypeDef NVIC_InitStructure;
//  
//    /* Ƕ�������жϿ�������ѡ�� */
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//  
//    /* ����USARTΪ�ж�Դ */
//    NVIC_InitStructure.NVIC_IRQChannel = MOD_USART_IRQ;
//    /* �������ȼ�Ϊ1 */
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//    /* �����ȼ�Ϊ1 */
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//    /* ʹ���ж� */
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    /* ��ʼ������NVIC */
//    NVIC_Init(&NVIC_InitStructure);
//}


 /**
  * @brief  Configures the USART Peripheral.
  * @param  None
  * @retval None
  */
void MOD_USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* Peripheral Clock Enable -------------------------------------------------*/
  /* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(MOD_USART_RX_GPIO_CLK | MOD_USART_TX_GPIO_CLK,ENABLE);

	/* Enable USART clock */
	RCC_APB1PeriphClockCmd(MOD_USART_CLK, ENABLE);
  
  /* USARTx GPIO configuration -----------------------------------------------*/   
  /* Configure USART Tx and Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = MOD_USART_TX_PIN  ;  
	GPIO_Init(MOD_USART_TX_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = MOD_USART_RX_PIN;
	GPIO_Init(MOD_USART_RX_GPIO_PORT, &GPIO_InitStructure);
  
	/* Connect USART pins to AF */
	GPIO_PinAFConfig(MOD_USART_RX_GPIO_PORT, MOD_USART_RX_SOURCE, MOD_USART_RX_AF);
	GPIO_PinAFConfig(MOD_USART_TX_GPIO_PORT, MOD_USART_TX_SOURCE, MOD_USART_TX_AF);
  
  /* USARTx configuration ----------------------------------------------------*/
  /* USARTx configured as follows:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - one Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = MOD_USART_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	/* When using Parity the word length must be configured to 9 bits */
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(MOD_USART, &USART_InitStructure); 
	
	/* Ƕ�������жϿ�����NVIC���� */
//	NVIC_Configuration();
  
	/* ʹ�ܴ��ڽ����ж� */
//	USART_ITConfig(MOD_USART, USART_IT_RXNE, ENABLE);
	
	/* Enable USART */
	USART_Cmd(MOD_USART, ENABLE);
}

/******************* END OF FILE ******************/
