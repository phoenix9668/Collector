#ifndef __FUNCTION_H
#define __FUNCTION_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./tim/bsp_basic_tim.h"
#include "./spi/bsp_spi.h"
#include "./key/bsp_key.h"
#include "./fsmc/bsp_fsmc.h" 
#include "./rtc/bsp_rtc.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define TX              0       // cc1101发送模式
#define RX              1       // cc1101接收模式
#define IDLE          	2       // cc1101空闲模式
#define ACK_LENGTH      24   		// 反馈数据包长度
#define ACK_CNT					ACK_LENGTH/6	// floor(ACK_LENGTH/6)
#define SEND_LENGTH     10			// 发送数据包长度
#define RECV_LENGTH   		17			// 接收数据包长度
#define RECV_TIMEOUT		2000  	// 接收等待2s
#define SEND_PACKAGE_NUM	80		// 发生数据包数
/* Exported macro ------------------------------------------------------------*/
extern __IO uint16_t	RecvWaitTime;  	// 接收等待时间
extern __IO uint8_t		RecvFlag;     	// =1接收等待时间结束，=0不处理

/* Exported functions ------------------------------------------------------- */
void MCU_Initial(void);
void RF_Initial(uint8_t addr, uint16_t sync, uint8_t mode);
void System_Initial(void);
void Function_Ctrl(uint8_t *commend);
void Check_Assign_RFID(uint8_t *commend);
void Check_All_RFID(uint8_t *commend);
void RF_SendPacket(uint8_t *commend, uint32_t rfid);
uint8_t	RF_Acknowledge(void);
void Reply_PC(uint8_t index);
void GetNandFlashAddr(flashInfo* pFlshInfo);

#endif /* __FUNCTION_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
