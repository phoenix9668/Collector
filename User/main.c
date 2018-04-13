/**
  ******************************************************************************
  * @file    CC1101_ctrl/main.c 
  * @author  phoenix
  * @version V1.0.0
  * @date    20-October-2017
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  ******************************************************************************  
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*===========================================================================
* 函数 : main() => 主函数，程序入口                                         *
* 说明 ：每1s发送一包数据，每包数据长度为10个字节，数据内容为0-9            *
         接收方反馈(应答)的数据长度为10个字节，数据内容为10-19              *
============================================================================*/
int main(void)
{
	Delay(0xfffff);
	System_Initial();
	Show_Message();
	
	while(1)
	{
		for (i=0; i<PCCOMMEND_LENGTH; i++) // clear array
		{PCCommend[i] = 0;}
		/* 等待串口接收数据完毕 */
		Usart_RecArray(DEBUG_USART, PCCommend);
		for(i=0; i<PCCOMMEND_LENGTH; i++)// for test
		{	
			printf("%x ",PCCommend[i]);
		}
		printf("\n");
		if(PCCommend[0] == 0xAB && PCCommend[1] == 0xCD)//begin index
		{
			Usart_SendString(DEBUG_USART,"start transfer\n");
			LED_STA_ON();
			Function_Ctrl(PCCommend);
		}
		else
		{
			Usart_SendString(DEBUG_USART,"package beginning error\n");
			LED_STA_OFF();
		}

	}
}

/**
  * @brief  Delay Function.
  * @param  nCount:specifies the Delay time length.
  * @retval None
  */
void Delay(__IO uint32_t nCount)
{
  while(nCount--)
  {
  }
}

/**
  * @brief  打印指令输入提示信息
  * @param  无
  * @retval 无
  */
static void Show_Message(void)
{   
	printf("\r\n CC1101 chip transfer performance test program \n");
	printf(" using USART3,configuration:%d 8-N-1 \n",DEBUG_USART_BAUDRATE);
	printf(" you need press USER button when you want transfer data\n");
	printf(" if choose transfer,the data must not exceed 60 bytes!!\n");
	printf(" PS: green led light when system in transfer mode\n");    
	printf("     orange led light when system in receive mode\n");
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
