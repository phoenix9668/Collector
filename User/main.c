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
uint8_t str1[16] = {'s','t','a','r','t',' ','t','r','a','n','s','f','e','r',0x0D,0x0A};
uint8_t str2[15] = {'f','r','a','m',' ','t','r','a','n','s','f','e','r',0x0D,0x0A};
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint32_t TimingDelay;
extern wiz_NetInfo gWIZNETINFO;
extern uint8_t buffer[2048];
extern uint8_t FRAM_Data[FRAM_DATA_LENGTH];
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*===========================================================================
* 函数 : main() => 主函数，程序入口                                         *
* 说明 ：每1s发送一包数据，每包数据长度为10个字节，数据内容为0-9            *
         接收方反馈(应答)的数据长度为10个字节，数据内容为10-19              *
============================================================================*/
int main(void)
{
	Delay(0xFFFFFF);
	System_Initial();
	#ifdef UART_DEBUG
		Show_Message();
	#endif
	if (SysTick_Config(SystemCoreClock / 1000))
  { 
    /* Capture error */ 
		Usart_SendString(MOD_USART,"SysTick_Config Error\n");
    while (1);
  }
	/* TIM6 configure 40ms*/
//	TIMx_Configuration();
	/* Check if the system has resumed from WWDG reset */
	WWDGRST_Clear();
	/* WWDG configuration */
	WWDG_Config();
	TimingDelay = 40;
	
	while(1)
	{
		for (i=0; i<PCCOMMAND_LENGTH; i++) //clear array
			{PCCommand[i] = 0;}
		#ifdef ETHERNET_ENABLE
			loopback_tcpc(0,PCCommand,gWIZNETINFO.gw,5000);
		#else
			Usart_RecArray(MOD_USART, PCCommand);/* 等待串口接收数据完毕 */
		#endif
		#ifdef UART_DEBUG
			for(i=0; i<PCCOMMAND_LENGTH; i++)//for test
			{
				printf("%x ",PCCommand[i]);
			}
			printf("\n");
		#endif
		if(PCCommand[0] == 0xAB && PCCommand[1] == 0xCD)//begin index
		{
			#ifdef ETHERNET_ENABLE
				Delay(0xFFFF);
				send_tcpc(0,str1,16,gWIZNETINFO.gw,5000);
			#else
				Usart_SendString(MOD_USART,"start transfer\n");
			#endif
			LED_STA_ON();
			Function_Ctrl(PCCommand);
			LED_STA_OFF();
		}
		else if(PCCommand[0] == 0xE5 && PCCommand[1] == 0x5E)
		{
			#ifdef ETHERNET_ENABLE
				Delay(0xFFFF);
				send_tcpc(0,str2,15,gWIZNETINFO.gw,5000);
			#else
				Usart_SendString(MOD_USART,"fram transfer\n");
			#endif
			LED_STA_ON();
			FRAM_Ctrl(PCCommand);
			LED_STA_OFF();
		}
		else
		{
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

#ifdef UART_DEBUG
/**
  * @brief  打印指令输入提示信息
  * @param  无
  * @retval 无
  */
static void Show_Message(void)
{
	uint16_t MBID;
	
	printf("\r\nmain program running\n");
	printf("using USART3,configuration:%d 8-N-1\n",MOD_USART_BAUDRATE);
	FM25L256Read(0x0, 7, FRAM_Data);
	MBID = (uint16_t)(0xFF00 & FRAM_Data[0]<<8)+(uint16_t)(0x00FF & FRAM_Data[1]);
	printf("Main_Board ID:%x\n",MBID);
}
#endif

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
