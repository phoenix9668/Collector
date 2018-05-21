/**
  ******************************************************************************
  * @file    bsp_rtc.c
  * @author  phoenix
  * @version V1.0
  * @date    29-January-2018
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */ 
#include "./rtc/bsp_rtc.h"
#include "./usart/bsp_com_usart.h"

/**
  * @brief  设置时间和日期
  * @param  无
  * @retval 无
  */
void RTC_TimeAndDate_Set(void)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	
	// 初始化时间
	RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
	RTC_TimeStructure.RTC_Hours = HOURS;        
	RTC_TimeStructure.RTC_Minutes = MINUTES;      
	RTC_TimeStructure.RTC_Seconds = SECONDS;      
	RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
	
  // 初始化日期	
	RTC_DateStructure.RTC_WeekDay = WEEKDAY;       
	RTC_DateStructure.RTC_Date = DATE;         
	RTC_DateStructure.RTC_Month = MONTH;         
	RTC_DateStructure.RTC_Year = YEAR;        
	RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

/**
  * @brief  设置时间和日期
  * @param  年，月，日，星期，时，分，秒
  * @retval 无
  */
void RTC_TimeAndDate_Reset(uint8_t year , uint8_t month, uint8_t date, uint8_t weekday, uint8_t hours, uint8_t minutes, uint8_t seconds)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	
	// 初始化时间
	RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
	RTC_TimeStructure.RTC_Hours = hours;
	RTC_TimeStructure.RTC_Minutes = minutes;
	RTC_TimeStructure.RTC_Seconds = seconds;
	if (RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure) == ERROR)
	{
		printf("RTC SetTime configure error\r\n");
	}
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
	
  // 初始化日期
	RTC_DateStructure.RTC_WeekDay = weekday;
	RTC_DateStructure.RTC_Date = date;
	RTC_DateStructure.RTC_Month = month;
	RTC_DateStructure.RTC_Year = year;
	if (RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure) == ERROR)
	{
		printf("RTC SetDate configure error\r\n");
	}
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

/**
  * @brief  获取时间和日期
  * @param  无
  * @retval 无
  */
void RTC_TimeAndDate_Access(RTC_TimeTypeDef*  RTC_TimeStructure, RTC_DateTypeDef* RTC_DateStructure)
{
	// 获取日历
	RTC_GetTime(RTC_Format_BIN, RTC_TimeStructure);
	RTC_GetDate(RTC_Format_BIN, RTC_DateStructure);
}

/**
  * @brief  显示时间和日期
  * @param  无
  * @retval 无
  */
void RTC_TimeAndDate_Show(void)
{
	uint8_t Rtctmp=0;
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	
	while(1)
	{
		// 获取日历
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	  RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
		
		// 每秒打印一次
  	if(Rtctmp != RTC_TimeStructure.RTC_Seconds)
    {
			// 打印日期
      printf("The Date:Y:20%0.2d - M:%0.2d - D:%0.2d - W:%0.2d\r\n", 
			RTC_DateStructure.RTC_Year,
			RTC_DateStructure.RTC_Month, 
			RTC_DateStructure.RTC_Date,
			RTC_DateStructure.RTC_WeekDay);
			// 打印时间
      printf("The Time:%0.2d:%0.2d:%0.2d\r\n", 
			RTC_TimeStructure.RTC_Hours, 
			RTC_TimeStructure.RTC_Minutes, 
			RTC_TimeStructure.RTC_Seconds);
      (void)RTC->DR;
    }
    Rtctmp = RTC_TimeStructure.RTC_Seconds;
	}	
}

/**
  * @brief  RTC配置：选择RTC时钟源，设置RTC_CLK的分频系数
  * @param  无
  * @retval 无
  */
void RTC_Config(void)
{  
	RTC_InitTypeDef RTC_InitStructure;
	
	/* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  /* PWR_CR:DBF置1，使能RTC、RTC备份寄存器和备份SRAM的访问 */
  PWR_BackupAccessCmd(ENABLE);
	/* Reset RTC Domain */
//	RCC_BackupResetCmd(ENABLE);
//  RCC_BackupResetCmd(DISABLE);

#if defined (RTC_CLOCK_SOURCE_LSI) 
  /* 使用LSI作为RTC时钟源会有误差
	 * 默认选择LSE作为RTC的时钟源
	 */
  /* Enable the LSI OSC */
  RCC_LSICmd(ENABLE);
  /* Wait till LSI is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET){}
  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

#elif defined (RTC_CLOCK_SOURCE_LSE)

  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET){}
  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);    

#endif /* RTC_CLOCK_SOURCE_LSI */

  /* 使能RTC时钟 */
  RCC_RTCCLKCmd(ENABLE);

  /* 等待 RTC APB 寄存器同步 */
  RTC_WaitForSynchro();
   
  /* Configure the RTC data register and RTC prescaler */
  /* ck_spre(1Hz) = RTCCLK(LSI) /(AsynchPrediv + 1)*(SynchPrediv + 1)*/
	/* 设置异步预分频器的值 */
	RTC_InitStructure.RTC_AsynchPrediv = ASYNCHPREDIV;
	/* 设置同步预分频器的值 */
	RTC_InitStructure.RTC_SynchPrediv = SYNCHPREDIV;
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24; 
	/* 用RTC_InitStructure的内容初始化RTC寄存器 */
	if (RTC_Init(&RTC_InitStructure) == ERROR)
	{
		printf("RTC configure error\r\n");
	}
}

/**
  * @brief  RTC配置检查： 当我们配置过RTC时间之后就往备份寄存器0写入一个数据做标记
	* 																						所以每次程序重新运行的时候就通过检测备份寄存器0的值来判断
	* 																						RTC 是否已经配置过，如果配置过那就继续运行，如果没有配置过
	* 																						就初始化RTC，配置RTC的时间
  * @param  无
  * @retval 无
  */
void RTC_Config_Check(void)
{
	if (RTC_ReadBackupRegister(RTC_BKP_DRX) != RTC_BKP_DATA)
  {
    /* 设置时间和日期 */
		RTC_TimeAndDate_Set();
  }
  else
  {
    /* 检查是否电源复位 */
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
      printf("power reset\r\n");
    }
    /* 检查是否外部复位 */
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
      printf("extern reset\r\n");
    }
    printf("no need to configure RTC\r\n");
    
    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    /* PWR_CR:DBF置1，使能RTC、RTC备份寄存器和备份SRAM的访问 */
    PWR_BackupAccessCmd(ENABLE);
    /* 等待 RTC APB 寄存器同步 */
    RTC_WaitForSynchro();   
  } 
}


/**********************************END OF FILE*************************************/
