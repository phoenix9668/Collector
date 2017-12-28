/**
  ******************************************************************************
  * @file    function.c 
  * @author  phoenix
  * @version V1.0.0
  * @date    25-September-2017
  * @brief   function program body
  ******************************************************************************
  * @attention
  *
  ******************************************************************************  
  */ 

/* Includes ------------------------------------------------------------------*/
#include "./function/function.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t	Chip_Addr	= 0;								// cc1101地址
uint8_t	RSSI = 0;											// RSSI值

extern uint8_t PCCommend[PCCOMMEND_LENGTH];	// 接收上位机命令数组
uint8_t SendBuffer[SEND_LENGTH] = {0};// 发送数据包
uint8_t RecvBuffer[RECV_LENGTH] = {0};// 接收数据包
uint8_t AckBuffer[ACK_LENGTH] = {0};	// 应答数据包
float ADC_ConvertedValueLocal[3];			// 用于保存转化计算后的电压值
/* Private function prototypes -----------------------------------------------*/
extern void Delay(__IO uint32_t nCount);
/* Private functions ---------------------------------------------------------*/

/*===========================================================================
* 函数 ：MCU_Initial() => 初始化CPU所有硬件                                 *
* 说明 ：关于所有硬件的初始化操作，已经被建成C库，见bsp.c文件               *
============================================================================*/
void MCU_Initial(void)
{ 
    Debug_USART_Config();   // 初始化串口
    GPIO_Config();         // 初始化GPIO
    Key_GPIO_Config();      // 初始化按键
    TIMx_Configuration();   // 初始化定时器6，0.5s 一个事件
    SPI_Config();          // 初始化SPI                           
}

/*===========================================================================
* 函数 ：RF_Initial() => 初始化RF芯片                                       *
* 输入 ：mode, =0,接收模式， else,发送模式                                  *
* 说明 ：CC1101的操作，已经被建成C库，见CC1101.c文件， 提供SPI和CSN操作，	*
         即可调用其内部所有函数用户无需再关心CC1101的寄存器操作问题。       *
============================================================================*/
void RF_Initial(uint8_t addr, uint16_t sync, uint8_t mode)
{
	CC1101Init(addr, sync);                       			// 初始化CC1101寄存器
	if(mode == RX)				{CC1101SetTRMode(RX_MODE);}		// 接收模式
	else if(mode == TX)		{CC1101SetTRMode(TX_MODE);}   // 发送模式
	else
	{
		CC1101SetIdle();																	// 空闲模式，以转到sleep状态
		CC1101WORInit();																	// 初始化电磁波激活功能
		CC1101SetWORMode();
	}
}

/*===========================================================================
* 函数: System_Initial() => 初始化系统所有外设                              *
============================================================================*/
void System_Initial(void)
{
    MCU_Initial();      // 初始化CPU所有硬件
    RF_Initial(0x5, 0xD391, RX);     // 初始化无线芯片,空闲模式
		LED_RUN_OFF();
		LED_STA_OFF();
		LED_COM_OFF();
}

/*===========================================================================
* 函数 ：Function_Ctrl() => 功能控制码解析和流程分解                        * 
============================================================================*/
void Function_Ctrl(uint8_t *commend)
{   
	if(commend[2] == 0x00 && commend[3] == 0x00)//判断设备编号
	{
		switch(((uint16_t)(0xFF00 & commend[4]<<8)+(uint16_t)(0x00FF & commend[5])))
		{
			/* 上位批量查询标签数据 */
			case 0xA0A0:	printf("no this function now\n");
										break;
			/* 上位发生时间同步数据 */
			case 0xA1A1:	printf("no this function now\n");
										break;
			/* 查询主控设备电源电量低标志、RTC电量低标志 */	
			case 0xA2A2:	printf("no this function now\n");
										break;
			/* 查询标签电量、加速度过小，加速度过大标志 */				
			case 0xA3A3:	printf("no this function now\n");
										break;
			/* 上位机查询指定编号标签 */
			case 0xA4A4:	Check_Assign_RFID(commend);
										break;
			default:	printf("function order error\n");
								break;
		}
	}
	else
	{
		printf("device number error\n");
	}		
}

/*===========================================================================
* 函数 ：Check_Assign_RFID() => 上位机查询指定编号标签                      * 
============================================================================*/
void Check_Assign_RFID(uint8_t *commend)
{   	
	uint32_t rfid;
	rfid = ((uint32_t)(0xFF000000 & commend[6]<<24)+(uint32_t)(0x00FF0000 & commend[7]<<16)+(uint32_t)(0x0000FF00 & commend[8]<<8)+(uint32_t)(0x000000FF & commend[9]));
	switch(rfid)
	{
		/* RFID coding is 1*/
		case 0x1:
			RF_Initial(0x5, 0xD391, RX);     // 初始化无线芯片
			RF_SendPacket(rfid);
			break;
		/* RFID coding is 2*/
		case 0x2:
			RF_Initial(0x6, 0x8289, RX);     // 初始化无线芯片
			RF_SendPacket(rfid);
			break;
		default:	printf("RFID coding error\n");
							break;
	}
		
}

/*===========================================================================
* 函数 : RF_SendPacket() => 无线发送数据函数                            *
* 输入 ：Sendbuffer指向待发送的数据，length发送数据长度                     *
* 输出 ：0，发送失败；else，发送成功                                        *
============================================================================*/
void RF_SendPacket(uint32_t rfid)
{
	uint8_t i=0;	
	
	for (i=0; i<SEND_LENGTH; i++) // clear array
		{SendBuffer[i] = 0;}
	
	SendBuffer[0] = 0xAB;
	SendBuffer[1] = 0xCD;
	SendBuffer[2] = 0x00;
	SendBuffer[3] = 0x00;
	SendBuffer[4] = 0xB4;
	SendBuffer[5] = 0xB4;
	SendBuffer[6] = (uint8_t)(0xff & rfid>>24);
	SendBuffer[7] = (uint8_t)(0xff & rfid>>16);
	SendBuffer[8] = (uint8_t)(0xff & rfid>>8);
	SendBuffer[9] = (uint8_t)(0xff & rfid);
		
	for(i=0; i<SEND_PACKAGE_NUM; i++)
	{
		CC1101SendPacket(SendBuffer, SEND_LENGTH, ADDRESS_CHECK);    // 发送数据
		Delay(0xFFFF);									// 计算得到平均27ms发送一次数据
//		Delay(0xFFFFF);								// 计算得到平均130ms发送一次数据
	}
	CC1101SetTRMode(RX_MODE);       	// 进入接收模式，等待应答
	Usart_SendString(DEBUG_USART,"Transmit OK\n");
//	RF_Initial(addr, sync, RX);
	RecvWaitTime = RECV_TIMEOUT;
	while(RF_Acknowledge() == 0 && RecvFlag == 0);
	RecvWaitTime = 0;
}

/*===========================================================================
* 函数 ：RF_Acknowledge() => 无线数据接收应答                               *
============================================================================*/
uint8_t	RF_Acknowledge(void)
{
	uint8_t i=0, length=0;
	uint8_t index;
	int16_t rssi_dBm;	
	
	if(CC1101_IRQ_READ() == 0)         // 检测无线模块是否产生接收中断 
	{
		while (CC1101_IRQ_READ() == 0);
		for (i=0; i<RECV_LENGTH; i++)   { RecvBuffer[i] = 0; } // clear array

		// 读取接收到的数据长度和数据内容
		length = CC1101RecPacket(RecvBuffer, &Chip_Addr, &RSSI);
		// 打印数据
		rssi_dBm = CC1101CalcRSSI_dBm(RSSI);
		printf("RSSI = %ddBm, length = %d, address = %d\n",rssi_dBm,length,Chip_Addr);
		rssi_dBm = CC1101CalcRSSI_dBm(RecvBuffer[14]);
		printf("RFID RSSI = %ddBm\n",rssi_dBm);
		for(i=0; i<RECV_LENGTH; i++)
		{	
			printf("%x ",RecvBuffer[i]);
		}
		printf("\n");		
		if(length == 0)
			{
				index = 1;
				Reply_PC(index);
				return 1;
			}
		else
			{
				if(RecvBuffer[0] == 0xAB && RecvBuffer[1] == 0xCD)
				{
					if(RecvBuffer[2] == 0xB0 && RecvBuffer[3] == 0xB0)
					{	
						index = 4;
						Reply_PC(index);
						return 4;
					}
					else if(RecvBuffer[2] == 0xC3 && RecvBuffer[3] == 0x01)
					{	
						index = 5;
						Reply_PC(index);
						return 5;
					}
					else if(RecvBuffer[2] == 0x01 && RecvBuffer[3] == 0x01)
					{	
						index = 6;
						Reply_PC(index);
						return 6;
					}
					else if(RecvBuffer[2] == 0x02 && RecvBuffer[3] == 0x02)
					{	
						index = 7;
						Reply_PC(index);
						return 7;
					}
					else if(RecvBuffer[2] == 0x03 && RecvBuffer[3] == 0x03)
					{	
						index = 8;
						Reply_PC(index);
						return 8;
					}
					else
					{	
						index = 3;
						Reply_PC(index);
						return 3;
					}
				}
				else
				{	
					index = 2;
					Reply_PC(index);
					return 2;
				}
			}
	}
	else	{	return 0;}
}

/*===========================================================================
* 函数 ：Reply_PC() => WIFI回复PC							                              *
============================================================================*/
void Reply_PC(uint8_t index)
{
	uint8_t i=0;	
		
	if(index == 4)
	{		
		AckBuffer[0] = 0xAB;
		AckBuffer[1] = 0xCD;
		AckBuffer[2] = PCCommend[2];
		AckBuffer[3] = PCCommend[2];
		AckBuffer[4] = 0xB4;
		AckBuffer[5] = 0xB4;
		for(i=0;i<ACK_LENGTH-7;i++)
		{
			AckBuffer[i+6] = RecvBuffer[i+4];
		}	
		AckBuffer[17] = RSSI;
		for(i=0; i<ACK_LENGTH; i++)
		{
			printf("%x ",AckBuffer[i]);
//			Usart_SendByte(DEBUG_USART, AckBuffer[i]);
		}
//		for(i=0; i<ACK_LENGTH; i++)
//		{	
//			printf("%x ",RecvBuffer[i]);
//		}
//		printf("\n");
//		for(i=0; i<ACK_CNT; i++)
//		{
//			ADC_ConvertedValueLocal[0] =(float)((((uint16_t)RecvBuffer[i*6+0]) + (0x0F00 & (((uint16_t)RecvBuffer[i*6+1])<<8)))*3.3/4096); 
//			ADC_ConvertedValueLocal[1] =(float)((((uint16_t)RecvBuffer[i*6+2]) + (0x0F00 & (((uint16_t)RecvBuffer[i*6+3])<<8)))*3.3/4096); 
//			ADC_ConvertedValueLocal[2] =(float)((((uint16_t)RecvBuffer[i*6+4]) + (0x0F00 & (((uint16_t)RecvBuffer[i*6+5])<<8)))*3.3/4096);  
//			printf("The current ADC1 value = %f V \r\n", ADC_ConvertedValueLocal[0]); 
//			printf("The current ADC2 value = %f V \r\n", ADC_ConvertedValueLocal[1]);
//			printf("The current ADC2 value = %f V \r\n", ADC_ConvertedValueLocal[2]);
//		}
	}
	else if(index == 5)
	{
		AckBuffer[0] = 0xAB;
		AckBuffer[1] = 0xCD;
		AckBuffer[2] = PCCommend[2];
		AckBuffer[3] = PCCommend[2];
		AckBuffer[4] = 0xB3;
		AckBuffer[5] = 0x01;
		for(i=0;i<ACK_LENGTH-7;i++)
		{
			AckBuffer[i+6] = RecvBuffer[i+4];
		}	
		AckBuffer[17] = RSSI;
		for(i=0; i<ACK_LENGTH; i++)
		{
			Usart_SendByte(DEBUG_USART, AckBuffer[i]);
		}
	}
	else if(index == 6)
	{
		AckBuffer[0] = 0xAB;
		AckBuffer[1] = 0xCD;
		AckBuffer[2] = PCCommend[2];
		AckBuffer[3] = PCCommend[2];
		AckBuffer[4] = 0x01;
		AckBuffer[5] = 0x01;
		for(i=0;i<ACK_LENGTH-7;i++)
		{
			AckBuffer[i+6] = RecvBuffer[i+4];
		}	
		AckBuffer[17] = RSSI;
		for(i=0; i<ACK_LENGTH; i++)
		{
			Usart_SendByte(DEBUG_USART, AckBuffer[i]);
		}
	}
	else if(index == 7)
	{
		AckBuffer[0] = 0xAB;
		AckBuffer[1] = 0xCD;
		AckBuffer[2] = PCCommend[2];
		AckBuffer[3] = PCCommend[2];
		AckBuffer[4] = 0x02;
		AckBuffer[5] = 0x02;
		for(i=0;i<ACK_LENGTH-7;i++)
		{
			AckBuffer[i+6] = RecvBuffer[i+4];
		}	
		AckBuffer[17] = RSSI;
		for(i=0; i<ACK_LENGTH; i++)
		{
			Usart_SendByte(DEBUG_USART, AckBuffer[i]);
		}
	}	
	else if(index == 8)
	{
		AckBuffer[0] = 0xAB;
		AckBuffer[1] = 0xCD;
		AckBuffer[2] = PCCommend[2];
		AckBuffer[3] = PCCommend[2];
		AckBuffer[4] = 0x03;
		AckBuffer[5] = 0x03;
		for(i=0;i<ACK_LENGTH-7;i++)
		{
			AckBuffer[i+6] = RecvBuffer[i+4];
		}	
		AckBuffer[17] = RSSI;
		for(i=0; i<ACK_LENGTH; i++)
		{
			Usart_SendByte(DEBUG_USART, AckBuffer[i]);
		}
	}
	else if(index == 1)
	{
		printf("receive error or Address Filtering fail\n");
	}
	else if(index == 2)
	{
		printf("RFID receive package beginning error\r\n");
	}
	else if(index == 3)
	{
		printf("RFID receive function order error\r\n");
	}
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
