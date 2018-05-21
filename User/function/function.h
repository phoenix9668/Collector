#ifndef __FUNCTION_H
#define __FUNCTION_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "./cc1101/cc1101.h"
#include "./usart/bsp_com_usart.h"
#include "./tim/bsp_basic_tim.h"
#include "./spi/bsp_spi.h"
#include "./fram/bsp_fram.h"
#include "./key/bsp_key.h"
#include "./fsmc/bsp_fsmc.h" 
#include "./rtc/bsp_rtc.h"
#include "./gprs/bsp_gprs.h"
#include "wizchip_conf.h"
#include "loopback.h"
#include "socket.h"
#include "w5500.h"
#include "spi.h"

//#define	ETHERNET_ENABLE

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define TX              0       // cc1101发送模式
#define RX              1       // cc1101接收模式
#define IDLE          	2       // cc1101空闲模式
#define RFID_SUM    		9       // RFID标签的总数
#define ACK_LENGTH      27   		// 反馈数据包长度
#define SEND_LENGTH     18			// 发送数据包长度
#define RECV_LENGTH   	19			// 接收数据包长度
#define RECV_TIMEOUT		44000000  			// 接收等待13s
#define SEND_PACKAGE_NUM	200		// 发生数据包数
#define FRAM_DATA_LENGTH 7			// 存储fram数据buffer
/* Exported macro ------------------------------------------------------------*/
extern __IO uint32_t	RecvWaitTime;  	// 接收等待时间
extern __IO uint8_t		RecvFlag;     	// =1接收等待时间结束，=0不处理

/* Exported functions ------------------------------------------------------- */
void MCU_Initial(void);
void RF_Initial(uint8_t addr, uint16_t sync, uint8_t mode);
void System_Initial(void);
void Function_Ctrl(uint8_t *command);
void Check_Assign_RFID(uint8_t *command);
void Check_All_RFID(uint8_t *command);
void Prog_Assign_RFID(uint8_t *command);
void Clear_All_RFID(uint8_t *command);
void RF_SendPacket(uint8_t *command);
void RF_SendClearPacket(uint8_t *command);
uint8_t	RF_Acknowledge(void);
void Reply_PC(uint8_t index);
void GetNandFlashAddr(flashInfo* pFlshInfo);
void ChipParametersConfiguration(void);
void NetworkParameterConfiguration(void);
void FRAM_Ctrl(uint8_t *command);
void Init_ID(void);

#endif /* __FUNCTION_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
