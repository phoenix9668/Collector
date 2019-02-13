/********************************************************************************
 Author : PH

 Date :   Janu, 2019

 File name :  bsp_wwdg.c 

 Description :	 STM32F407 WWDG driver                

 Hardware plateform : 	STM32F407
 
********************************************************************************/

#include "./wwdg/bsp_wwdg.h"
#include "./gprs/bsp_gprs.h"
#include "./usart/bsp_com_usart.h"

/*******************************************************************
  @brief  WWDG_Config function
  @param  None
  @retval None
*******************************************************************/
void WWDG_Config(void)
{
  /* Enable WWDG clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

  /* WWDG clock counter = (PCLK1 (42MHz)/4096)/8 = 1281 Hz (~780 us)  */
  WWDG_SetPrescaler(WWDG_Prescaler_8);

  /* Set Window value to 90; WWDG counter should be refreshed only when the counter
    is below 90 (and greater than 64) otherwise a reset will be generated */
  WWDG_SetWindowValue(100);

  /* Enable WWDG and set counter value to 127, WWDG timeout = ~780 us * 64 = 49.92 ms 
     In this case the refresh window is: 
           ~780 * (127-100) = 21.06ms < refresh window < ~780 * 64 = 49.9ms
  */
  WWDG_Enable(127);
}

/*******************************************************************
  @brief  WWDG_Refresh function
  @param  None
  @retval None
*******************************************************************/
void WWDG_Refresh(void)
{
	WWDG_SetCounter(127);
}

/*******************************************************************
  @brief  WWDGRST_Clear function
  @param  None
  @retval None
*******************************************************************/
void WWDGRST_Clear(void)
{
  /* Check if the system has resumed from WWDG reset */
  if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET)
  { 
    /* WWDGRST flag set */
		Usart_SendString(MOD_USART,"system has resumed from WWDG reset\n");
    /* Clear reset flags */
    RCC_ClearFlag();
  }
}
