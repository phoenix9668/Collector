#ifndef __RTC_H__
#define __RTC_H__

#include "stm32f4xx.h"

// 时钟源宏定义
#define RTC_CLOCK_SOURCE_LSE
//#define RTC_CLOCK_SOURCE_LSI

// 异步分频因子
#define ASYNCHPREDIV		0X7F
// 同步分频因子
#define SYNCHPREDIV		0XFF

// 时间宏定义
#define RTC_H12_AMorPM		RTC_H12_AM  
#define HOURS												0          // 0~23
#define MINUTES              			0          // 0~59
#define SECONDS              			0          // 0~59

// 日期宏定义
#define WEEKDAY              			0         // 1~7
#define DATE                 						0         // 1~31
#define MONTH                				0         // 1~12
#define YEAR                 						0         // 0~99

// 时间格式宏定义
#define RTC_Format_BINorBCD		RTC_Format_BIN

// 备份域寄存器宏定义
#define RTC_BKP_DRX		RTC_BKP_DR0
// 写入到备份寄存器的数据宏定义
#define RTC_BKP_DATA		0X32F2
 
void RTC_Config(void);
void RTC_Config_Check(void);
void RTC_TimeAndDate_Set(void);
void RTC_TimeAndDate_Reset(uint8_t year , uint8_t month, uint8_t date, uint8_t weekday, uint8_t hours, uint8_t minutes, uint8_t seconds);
void RTC_TimeAndDate_Show(void);
void RTC_TimeAndDate_Access(RTC_TimeTypeDef*  RTC_TimeStructure, RTC_DateTypeDef* RTC_DateStructure);

#endif // __RTC_H__
