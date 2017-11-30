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
#include "./usart/bsp_debug_usart.h"
#include "./tim/bsp_basic_tim.h"
#include "./spi/bsp_spi.h"
#include "./adc/bsp_adc.h"

/** @addtogroup STM32F4_Discovery_Peripheral_Examples
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
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
void DEBUG_USART_IRQHandler(void)
{
  uint8_t ucTemp;
	if(USART_GetITStatus(DEBUG_USART,USART_IT_RXNE)!=RESET)
	{		
		ucTemp = USART_ReceiveData( DEBUG_USART );
        USART_SendData(DEBUG_USART,ucTemp);
	}	 
}	

void BASIC_TIM_IRQHandler(void)
{
	if (TIM_GetITStatus(BASIC_TIM, TIM_IT_Update) != RESET ) 
	{	
        if(RecvWaitTime != 0 && RecvWaitTime != 1)												// 数据接收计时
					{	RecvWaitTime--;}
				else if(RecvWaitTime == 1)
					{	RecvFlag=1;}
				else
					{	RecvFlag=0;}
        
        if(SendTime != 0)                           // 1ms时间到，置位SendFlag标志，主函数查询发送数据    
        { 
            if(--SendTime == 0)    
                {   SendTime=SEND_GAP; 
                    SendFlag=1;
                    LED6_Blue_TOG();
										MMA7361L_ReadHandler();
                }
        } 
        TIM_ClearITPendingBit(BASIC_TIM, TIM_IT_Update);  	      
	}		 	
}

//void CC1101_GDO2_IRQHandler(void)
//{
//  //是否产生EXTI Line中断
//	if(EXTI_GetITStatus(CC1101_GDO2_EXTI_LINE) != RESET) 
//	{	
//		LED5_Red_TOG();
//    //清除中断标志位
//		EXTI_ClearITPendingBit(CC1101_GDO2_EXTI_LINE);     
//	}  
//}

void MMA7361L_ReadHandler(void)
{
	uint16_t i;
	MMA7361L_GS_1G5();
	MMA7361L_SL_OFF();
	for(i=0; i<600; i++)
	if(CollectCnt < ACK_CNT)
		{
			AckBuffer[CollectCnt*6] = 0xFF & ADC_ConvertedValue[0];
			AckBuffer[CollectCnt*6 + 1] = ((0x0F & (ADC_ConvertedValue[0]>>8)) + (0xF0 & ((uint8_t)CollectCnt<<4)));
			AckBuffer[CollectCnt*6 + 2] = 0xFF & ADC_ConvertedValue[1];
			AckBuffer[CollectCnt*6 + 3] = 0xFF & (ADC_ConvertedValue[1]>>8);
			AckBuffer[CollectCnt*6 + 4] = 0xFF & ADC_ConvertedValue[2];
			AckBuffer[CollectCnt*6 + 5] = 0xFF & (ADC_ConvertedValue[2]>>8);
		}
		
//	printf("The current AD1 value = 0x%08X \r\n", ((uint16_t)(AckBuffer[CollectCnt*6 + 1])<<8) + (uint16_t)AckBuffer[CollectCnt*6]); 
//	printf("The current AD2 value = 0x%08X \r\n", ((uint16_t)(AckBuffer[CollectCnt*6 + 3])<<8) + (uint16_t)AckBuffer[CollectCnt*6 + 2]);
//	printf("The current AD3 value = 0x%08X \r\n", ((uint16_t)(AckBuffer[CollectCnt*6 + 5])<<8) + (uint16_t)AckBuffer[CollectCnt*6 + 4]);   
   
	MMA7361L_SL_ON();
	CollectCnt++;
//	printf("CollectCnt = %d\r\n", CollectCnt-1);
	if(CollectCnt == ACK_CNT)
		{
			CollectCnt = 0;
//			for(i=0; i<ACK_LENGTH; i++) // clear array
//				{AckBuffer[i] = 0;}	
		}
}


/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
