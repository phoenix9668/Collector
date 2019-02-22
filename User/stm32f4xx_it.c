/**
  ******************************************************************************
  * @file    IO_Toggle/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    19-September-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "./function/function.h"

/** @addtogroup STM32F4_Discovery_Peripheral_Examples
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO ITStatus RFReady = RESET;
uint8_t wwdg_flag;
extern uint32_t TimingDelay;
extern __IO FlagStatus TxRxState;
extern __IO uint8_t cnt_i,cnt_k,cnt_j;
extern uint8_t SendBuffer[SEND_LENGTH];
extern uint8_t RecvBuffer[RECV_LENGTH];
/* Private function prototypes -----------------------------------------------*/
extern void Delay(__IO uint32_t nCount);
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	if(wwdg_flag == 0 || wwdg_flag == 1)
	{
		TimingDelay_Decrement();
		if(TimingDelay == 0)
		{
			LED_RUN_TOG();
			WWDG_Refresh();
			wwdg_flag = 1;
			TimingDelay = 35;
		}
	}
	else if(wwdg_flag == 2)
	{
		TimingDelay_Decrement();
	}
	GPRSTiming_Decrement();

}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
void COM1_USART_IRQHandler(void)
{
  uint8_t ucTemp;
	if(USART_GetITStatus(COM1_USART,USART_IT_RXNE)!=RESET)
	{		
		ucTemp = USART_ReceiveData(COM1_USART);
		USART_SendData(COM1_USART,ucTemp);
	}
}

/*
void drv_gsm_tx(uint8_t *data,uint16_t size)
{
    //等待空闲
    while (Flag_Tx_Gsm_Busy);
    Flag_Tx_Gsm_Busy = 1;
    //设置传输数据长度
    DMA_SetCurrDataCounter(USART5_TX_DMA_STREAM,size);
    //打开DMA,开始发送
    DMA_Cmd(USART5_TX_DMA_STREAM,ENABLE);
}

void USART5_DMA_TX_IRQHandler(void)  
{
	printf("oh my god");
	if(DMA_GetITStatus(USART5_TX_DMA_STREAM,DMA_IT_TCIF7) != RESET)   
	{
		//清除标志位
		DMA_ClearFlag(USART5_TX_DMA_STREAM,DMA_FLAG_TCIF7);  
		//关闭DMA
		DMA_Cmd(USART5_TX_DMA_STREAM,DISABLE);  
		//打开发送完成中断，发送最后两个字节
		USART_ITConfig(COM1_USART,USART_IT_TC,ENABLE);  
	}
}
  
void COM1_USART_IRQHandler(void)
{
	uint16_t len;
	uint8_t i = 0;

	//发送完成中断处理
	if(USART_GetITStatus(COM1_USART, USART_IT_TC) != RESET)  
	{
		//关闭发送完成中断
		USART_ITConfig(COM1_USART,USART_IT_TC,DISABLE);  
		//发送完成
		Flag_Tx_Gsm_Busy = 0;
	}
      
	//接收完成中断处理
	len = drv_gsm_deal_irq_rx_end(aRxBuffer);
	if (len != 0)
	{
		printf("len = %d",len);
		for(i = 0; i < len; i++)
		{
			printf("%d ",aRxBuffer[i]);
		}
	}
}

uint8_t drv_gsm_deal_irq_rx_end(uint8_t *buf)
{
	uint16_t len = 0;  
      
	//接收完成中断
	if(USART_GetITStatus(COM1_USART, USART_IT_IDLE) != RESET)  
	{
		COM1_USART->SR;  
		COM1_USART->DR; //清USART_IT_IDLE标志
		//关闭DMA  
		DMA_Cmd(USART5_RX_DMA_STREAM,DISABLE);  
		//清除标志位
		DMA_ClearFlag(USART5_RX_DMA_STREAM,DMA_FLAG_TCIF5);  
          
		//获得接收帧帧长
		len = RX_LEN_GSM - DMA_GetCurrDataCounter(USART5_RX_DMA_STREAM);
		//设置传输数据长度
		DMA_SetCurrDataCounter(USART5_RX_DMA_STREAM,RX_LEN_GSM);  
		//打开DMA  
		DMA_Cmd(USART5_RX_DMA_STREAM,ENABLE);  
  
		return len;
	}
	return 0;  
}
*/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
void COM2_USART_IRQHandler(void)
{
  uint8_t ucTemp;
	if(USART_GetITStatus(COM2_USART,USART_IT_RXNE)!=RESET)
	{		
		ucTemp = USART_ReceiveData(COM2_USART);
		USART_SendData(COM2_USART,ucTemp);
	}
}

/*
void BASIC_TIM_IRQHandler(void)
{
	if (TIM_GetITStatus(BASIC_TIM, TIM_IT_Update) != RESET ) 
	{
		LED_RUN_TOG();
		WWDG_Refresh();
		wwdg_flag = 1;
		TIM_ClearITPendingBit(BASIC_TIM, TIM_IT_Update);
	}
}
*/

void CC1101_IRQ_IRQHandler(void)
{
	if(EXTI_GetITStatus(CC1101_IRQ_EXTI_LINE) != RESET) 
	{
		if(TxRxState == SET)
		{
			RFReady = SET;
		}
		else if(TxRxState == RESET)
		{
			RFReady = SET;
			Delay(0xFFFF);
		}
    /* Clear the EXTI line 1 pending bit */
		EXTI_ClearITPendingBit(CC1101_IRQ_EXTI_LINE);     
	}  
}

void CC1101_GDO2_IRQHandler(void)
{
	if(EXTI_GetITStatus(CC1101_GDO2_EXTI_LINE) != RESET) 
	{
		if(TxRxState == SET)
		{
			cnt_i++;
			if(cnt_i == cnt_k)
			{
				CC1101WriteMultiReg(CC1101_TXFIFO, (SendBuffer+60*cnt_k), cnt_j);
			}
			else
			{
				CC1101WriteMultiReg(CC1101_TXFIFO, (SendBuffer+60*cnt_i), 60);
			}
		}
		else if(TxRxState == RESET)
		{
			if(cnt_i != cnt_k)
			{
				CC1101ReadMultiReg(CC1101_RXFIFO, (RecvBuffer+60*cnt_i), 60);// Pull data
				cnt_i++;
			}
		}

    /* Clear the EXTI line 1 pending bit */
		EXTI_ClearITPendingBit(CC1101_GDO2_EXTI_LINE);     
	}  
}


/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
