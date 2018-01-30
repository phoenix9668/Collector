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
RTC_TimeTypeDef	rtc_timestructure;
RTC_DateTypeDef		rtc_datestructure;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t	Chip_Addr	= 0;								// cc1101地址
uint8_t	RSSI = 0;											// RSSI值
flashInfo		flshInfo;
NAND_IDTypeDef		NAND_ID;
uint8_t	gBuff[300] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
uint8_t	buff[300] = {0};

extern uint8_t PCCommend[PCCOMMEND_LENGTH];	// 接收上位机命令数组
uint8_t SendBuffer[SEND_LENGTH] = {0};// 发送数据包
uint8_t RecvBuffer[RECV_LENGTH] = {0};// 接收数据包
uint8_t AckBuffer[ACK_LENGTH] = {0};	// 应答数据包
float ADC_ConvertedValueLocal[3];			// 用于保存转化计算后的电压值
/* Private function prototypes -----------------------------------------------*/
extern void Delay(__IO uint32_t nCount);
/* Private functions ---------------------------------------------------------*/

/*===========================================================================
* 函数 ：MCU_Initial() => 初始化CPU所有硬件                                 																											*
* 说明 ：关于所有硬件的初始化操作，已经被建成C库，见bsp.c文件              	 														*
============================================================================*/
void MCU_Initial(void)
{ 
    GPIO_Config();         					// 初始化GPIO
		Debug_USART_Config();  // 初始化串口
    Key_GPIO_Config();      		// 初始化按键
    TIMx_Configuration();   		// 初始化定时器6，0.5s 一个事件
    SPI_Config();          							// 初始化SPI
		RTC_Config();											// 初始化RTC
//		FSMC_GPIO_Config();			// 初始化FSMC接口
//		FSMC_NAND_Config();			// 初始化FSMC接口
}

/*===========================================================================
* 函数 ：RF_Initial() => 初始化RF芯片                                       																																*
* 输入 ：mode, =0,接收模式， else,发送模式                                  																												*
* 说明 ：CC1101的操作，已经被建成C库，见CC1101.c文件， 提供SPI和CSN操作，									*
         即可调用其内部所有函数用户无需再关心CC1101的寄存器操作问题。       													*
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
* 函数: System_Initial() => 初始化系统所有外设                              																												*
============================================================================*/
void System_Initial(void)
{
//		uint16_t i;
//		uint32_t flshStatus;
    MCU_Initial();      // 初始化CPU所有硬件
		RTC_Config_Check();      // 检查RTC，判断是否需要重新配置
    RF_Initial(0x5, 0xD391, RX);     // 初始化无线芯片,空闲模式
//		RTC_TimeAndDate_Show();
	
//		FSMC_NAND_ReadID(&NAND_ID);
//		printf("%x,%x,%x,%x,%x",NAND_ID.Maker_ID,NAND_ID.Device_ID,NAND_ID.Third_ID,NAND_ID.Fourth_ID,NAND_ID.Fifth_ID);
//	
//		GetNandFlashAddr(&flshInfo);
//		// A new block.we erase it before use it.
//		if(flshInfo.pageNo == 0)
//		flshStatus = FlashBlockErase(flshInfo);
//		printf("flshStatus = %x",flshStatus);
//		// program 1 page data
//		flshStatus = FlashSecPageProgram(flshInfo, gBuff, 30);
//		printf("flshStatus = %x",flshStatus);
//		// read 1 page data
//		flshStatus = FlashSecPageRead(flshInfo, buff, 30);
//		printf("flshStatus = %x",flshStatus);
//		for(i=0; i<30; i++)
//		{
//			printf("%x ",buff[i]);
//		}
}

/*===========================================================================
* 函数 ：Function_Ctrl() => 功能控制码解析和流程分解                        																							* 
============================================================================*/
void Function_Ctrl(uint8_t *commend)
{   
	if(commend[2] == 0x00 && commend[3] == 0x00)//判断设备编号
	{
		switch(((uint16_t)(0xFF00 & commend[4]<<8)+(uint16_t)(0x00FF & commend[5])))
		{
			/* 上位批量查询标签数据 */
			case 0xA0A0:	
										Check_All_RFID(commend);
										break;
			/* 上位发生时间同步数据 */
			case 0xA101:	
										RTC_TimeAndDate_Reset(commend[10],  commend[11],  commend[12],  commend[13],  commend[14],  commend[15],  commend[16]);
										RTC_Config_Check();      // 检查RTC，判断是否需要重新配置
										Reply_PC(10);
										break;
			/* 读取同步数据 */
			case 0xA102:
										Reply_PC(10);
										break;
			/* 查询主控设备电源电量低标志、RTC电量低标志 */
			case 0xA2A2:	
										printf("no this function now\n");
										break;
			/* 查询标签电量、加速度过小，加速度过大标志 */
			case 0xA3A3:	
										printf("no this function now\n");
										break;
			/* 上位机查询指定编号标签 */
			case 0xA4A4:	
										Check_Assign_RFID(commend);
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
* 函数 Check_All_RFID() => 上位机查询指定编号标签                      																										* 
============================================================================*/
void Check_All_RFID(uint8_t *commend)
{   	
	uint32_t rfid;
	rfid = ((uint32_t)(0xFF000000 & commend[6]<<24)+(uint32_t)(0x00FF0000 & commend[7]<<16)+(uint32_t)(0x0000FF00 & commend[8]<<8)+(uint32_t)(0x000000FF & commend[9]));
	/* RFID coding is 1*/
	RF_Initial(0x35, 0x4D5A, RX);     // 初始化无线芯片
	RF_SendPacket(commend, rfid);
	Delay(0xFFFF);
	rfid = rfid + (uint32_t)1;
	/* RFID coding is 2*/
	RF_Initial(0x36, 0x6D7A, RX);     // 初始化无线芯片
	RF_SendPacket(commend, rfid);
	Delay(0xFFFF);
}

/*===========================================================================
* 函数 ：Check_Assign_RFID() => 上位机查询指定编号标签                      																				* 
============================================================================*/
void Check_Assign_RFID(uint8_t *commend)
{   	
	uint32_t rfid;
	rfid = ((uint32_t)(0xFF000000 & commend[6]<<24)+(uint32_t)(0x00FF0000 & commend[7]<<16)+(uint32_t)(0x0000FF00 & commend[8]<<8)+(uint32_t)(0x000000FF & commend[9]));
	switch(rfid)
	{
		/* RFID coding is 1*/
		case 0x1:
			RF_Initial(0x35, 0x4D5A, RX);     // 初始化无线芯片
			RF_SendPacket(commend, rfid);
			break;
		/* RFID coding is 2*/
		case 0x2:
			RF_Initial(0x36, 0x6D7A, RX);     // 初始化无线芯片
			RF_SendPacket(commend, rfid);
			break;
		default:	
			printf("RFID coding error\n");
			break;
	}
		
}

/*===========================================================================
* 函数 : RF_SendPacket() => 无线发送数据函数                            																													*
* 输入 ：Sendbuffer指向待发送的数据，length发送数据长度                     																			*
* 输出 ：0，发送失败；else，发送成功                                        																													*
============================================================================*/
void RF_SendPacket(uint8_t *commend, uint32_t rfid)
{
	uint8_t i=0;
	
	for (i=0; i<SEND_LENGTH; i++) // clear array
		{SendBuffer[i] = 0;}
	
	if(commend[4] == 0xA0 && commend[5] == 0xA0)
	{
		SendBuffer[0] = 0xAB;
		SendBuffer[1] = 0xCD;
		SendBuffer[2] = commend[2];
		SendBuffer[3] = commend[3];
		SendBuffer[4] = 0xC0;
		SendBuffer[5] = 0xC0;
		SendBuffer[6] = (uint8_t)(0xff & rfid>>24);
		SendBuffer[7] = (uint8_t)(0xff & rfid>>16);
		SendBuffer[8] = (uint8_t)(0xff & rfid>>8);
		SendBuffer[9] = (uint8_t)(0xff & rfid);
	}
	else if(commend[4] == 0xA4 && commend[5] == 0xA4)
	{
		SendBuffer[0] = 0xAB;
		SendBuffer[1] = 0xCD;
		SendBuffer[2] = commend[2];
		SendBuffer[3] = commend[3];
		SendBuffer[4] = 0xC4;
		SendBuffer[5] = 0xC4;
		SendBuffer[6] = commend[6];
		SendBuffer[7] = commend[7];
		SendBuffer[8] = commend[8];
		SendBuffer[9] = commend[9];
	}

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
* 函数 ：RF_Acknowledge() => 无线数据接收应答                               													*
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
					if(RecvBuffer[4] == 0xD0 && RecvBuffer[5] == 0x01)
					{
						index = 4;
						Reply_PC(index);
						return 4;
					}
					else if(RecvBuffer[4] == 0xD3 && RecvBuffer[5] == 0x01)
					{
						index = 5;
						Reply_PC(index);
						return 5;
					}
					else if(RecvBuffer[4] == 0xD4 && RecvBuffer[5] == 0x01)
					{
						index = 6;
						Reply_PC(index);
						return 6;
					}
					else if(RecvBuffer[4] == 0x01 && RecvBuffer[5] == 0x01)
					{
						index = 7;
						Reply_PC(index);
						return 7;
					}
					else if(RecvBuffer[4] == 0x02 && RecvBuffer[5] == 0x02)
					{
						index = 8;
						Reply_PC(index);
						return 8;
					}
					else if(RecvBuffer[4] == 0x03 && RecvBuffer[5] == 0x03)
					{
						index = 9;
						Reply_PC(index);
						return 9;
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
* 函数 ：Reply_PC() => WIFI回复PC							                              																															*
============================================================================*/
void Reply_PC(uint8_t index)
{
	uint8_t i=0;	
		
	if(index == 4)
	{
		RTC_TimeAndDate_Access(&rtc_timestructure, &rtc_datestructure);
		AckBuffer[0] = 0xAB;
		AckBuffer[1] = 0xCD;
		AckBuffer[2] = RecvBuffer[2];
		AckBuffer[3] = RecvBuffer[3];
		AckBuffer[4] = 0xB0;
		AckBuffer[5] = 0x01;
		for(i=0;i<ACK_LENGTH-14;i++)
		{
			AckBuffer[i+6] = RecvBuffer[i+6];
		}
		AckBuffer[16] = rtc_datestructure.RTC_Year;
		AckBuffer[17] = rtc_datestructure.RTC_Month;
		AckBuffer[18] = rtc_datestructure.RTC_Date;
		AckBuffer[19] = rtc_datestructure.RTC_WeekDay;
		AckBuffer[20] = rtc_timestructure.RTC_Hours;
		AckBuffer[21] = rtc_timestructure.RTC_Minutes;
		AckBuffer[22] = rtc_timestructure.RTC_Seconds;
		AckBuffer[23] = RSSI;
		for(i=0; i<ACK_LENGTH; i++)
		{
			printf("%x ",AckBuffer[i]);
//			Usart_SendByte(DEBUG_USART, AckBuffer[i]);
		}
		printf("\n");
		
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
		RTC_TimeAndDate_Access(&rtc_timestructure, &rtc_datestructure);
		AckBuffer[0] = 0xAB;
		AckBuffer[1] = 0xCD;
		AckBuffer[2] = RecvBuffer[2];
		AckBuffer[3] = RecvBuffer[3];
		AckBuffer[4] = 0xB3;
		AckBuffer[5] = 0x01;
		for(i=0;i<ACK_LENGTH-14;i++)
		{
			AckBuffer[i+6] = RecvBuffer[i+6];
		}
		AckBuffer[16] = rtc_datestructure.RTC_Year;
		AckBuffer[17] = rtc_datestructure.RTC_Month;
		AckBuffer[18] = rtc_datestructure.RTC_Date;
		AckBuffer[19] = rtc_datestructure.RTC_WeekDay;
		AckBuffer[20] = rtc_timestructure.RTC_Hours;
		AckBuffer[21] = rtc_timestructure.RTC_Minutes;
		AckBuffer[22] = rtc_timestructure.RTC_Seconds;
		AckBuffer[23] = RSSI;
		for(i=0; i<ACK_LENGTH; i++)
		{
			Usart_SendByte(DEBUG_USART, AckBuffer[i]);
		}
	}
	else if(index == 6)
	{		
		RTC_TimeAndDate_Access(&rtc_timestructure, &rtc_datestructure);
		AckBuffer[0] = 0xAB;
		AckBuffer[1] = 0xCD;
		AckBuffer[2] = RecvBuffer[2];
		AckBuffer[3] = RecvBuffer[3];
		AckBuffer[4] = 0xB4;
		AckBuffer[5] = 0x01;
		for(i=0;i<ACK_LENGTH-14;i++)
		{
			AckBuffer[i+6] = RecvBuffer[i+6];
		}
		AckBuffer[16] = rtc_datestructure.RTC_Year;
		AckBuffer[17] = rtc_datestructure.RTC_Month;
		AckBuffer[18] = rtc_datestructure.RTC_Date;
		AckBuffer[19] = rtc_datestructure.RTC_WeekDay;
		AckBuffer[20] = rtc_timestructure.RTC_Hours;
		AckBuffer[21] = rtc_timestructure.RTC_Minutes;
		AckBuffer[22] = rtc_timestructure.RTC_Seconds;
		AckBuffer[23] = RSSI;
		for(i=0; i<ACK_LENGTH; i++)
		{
			printf("%x ",AckBuffer[i]);
//			Usart_SendByte(DEBUG_USART, AckBuffer[i]);
		}
		printf("\n");	
	}	
	else if(index == 7)
	{
		RTC_TimeAndDate_Access(&rtc_timestructure, &rtc_datestructure);
		AckBuffer[0] = 0xAB;
		AckBuffer[1] = 0xCD;
		AckBuffer[2] = RecvBuffer[2];
		AckBuffer[3] = RecvBuffer[3];
		AckBuffer[4] = 0x01;
		AckBuffer[5] = 0x01;
		for(i=0;i<ACK_LENGTH-14;i++)
		{
			AckBuffer[i+6] = RecvBuffer[i+6];
		}
		AckBuffer[16] = rtc_datestructure.RTC_Year;
		AckBuffer[17] = rtc_datestructure.RTC_Month;
		AckBuffer[18] = rtc_datestructure.RTC_Date;
		AckBuffer[19] = rtc_datestructure.RTC_WeekDay;
		AckBuffer[20] = rtc_timestructure.RTC_Hours;
		AckBuffer[21] = rtc_timestructure.RTC_Minutes;
		AckBuffer[22] = rtc_timestructure.RTC_Seconds;
		AckBuffer[23] = RSSI;
		for(i=0; i<ACK_LENGTH; i++)
		{
			Usart_SendByte(DEBUG_USART, AckBuffer[i]);
		}
	}
	else if(index == 8)
	{
		RTC_TimeAndDate_Access(&rtc_timestructure, &rtc_datestructure);
		AckBuffer[0] = 0xAB;
		AckBuffer[1] = 0xCD;
		AckBuffer[2] = RecvBuffer[2];
		AckBuffer[3] = RecvBuffer[2];
		AckBuffer[4] = 0x02;
		AckBuffer[5] = 0x02;
		for(i=0;i<ACK_LENGTH-14;i++)
		{
			AckBuffer[i+6] = RecvBuffer[i+6];
		}
		AckBuffer[16] = rtc_datestructure.RTC_Year;
		AckBuffer[17] = rtc_datestructure.RTC_Month;
		AckBuffer[18] = rtc_datestructure.RTC_Date;
		AckBuffer[19] = rtc_datestructure.RTC_WeekDay;
		AckBuffer[20] = rtc_timestructure.RTC_Hours;
		AckBuffer[21] = rtc_timestructure.RTC_Minutes;
		AckBuffer[22] = rtc_timestructure.RTC_Seconds;
		AckBuffer[23] = RSSI;
		for(i=0; i<ACK_LENGTH; i++)
		{
			Usart_SendByte(DEBUG_USART, AckBuffer[i]);
		}
	}	
	else if(index == 9)
	{
		RTC_TimeAndDate_Access(&rtc_timestructure, &rtc_datestructure);
		AckBuffer[0] = 0xAB;
		AckBuffer[1] = 0xCD;
		AckBuffer[2] = RecvBuffer[2];
		AckBuffer[3] = RecvBuffer[2];
		AckBuffer[4] = 0x03;
		AckBuffer[5] = 0x03;
		for(i=0;i<ACK_LENGTH-14;i++)
		{
			AckBuffer[i+6] = RecvBuffer[i+6];
		}
		AckBuffer[16] = rtc_datestructure.RTC_Year;
		AckBuffer[17] = rtc_datestructure.RTC_Month;
		AckBuffer[18] = rtc_datestructure.RTC_Date;
		AckBuffer[19] = rtc_datestructure.RTC_WeekDay;
		AckBuffer[20] = rtc_timestructure.RTC_Hours;
		AckBuffer[21] = rtc_timestructure.RTC_Minutes;
		AckBuffer[22] = rtc_timestructure.RTC_Seconds;
		AckBuffer[23] = RSSI;
		for(i=0; i<ACK_LENGTH; i++)
		{
			Usart_SendByte(DEBUG_USART, AckBuffer[i]);
		}
	}
	else if(index == 10)
	{
		RTC_TimeAndDate_Access(&rtc_timestructure, &rtc_datestructure);
		AckBuffer[0] = 0xAB;
		AckBuffer[1] = 0xCD;
		AckBuffer[2] = PCCommend[2];
		AckBuffer[3] = PCCommend[3];
		AckBuffer[4] = 0xB1;
		AckBuffer[5] = PCCommend[5];
		AckBuffer[6] = PCCommend[6];
		AckBuffer[7] = PCCommend[7];
		AckBuffer[8] = PCCommend[8];
		AckBuffer[9] = PCCommend[9];
		for(i=0;i<6;i++)
		{
			AckBuffer[i+10] = 0;
		}
		AckBuffer[16] = rtc_datestructure.RTC_Year;
		AckBuffer[17] = rtc_datestructure.RTC_Month;
		AckBuffer[18] = rtc_datestructure.RTC_Date;
		AckBuffer[19] = rtc_datestructure.RTC_WeekDay;
		AckBuffer[20] = rtc_timestructure.RTC_Hours;
		AckBuffer[21] = rtc_timestructure.RTC_Minutes;
		AckBuffer[22] = rtc_timestructure.RTC_Seconds;
		for(i=0; i<ACK_LENGTH; i++)
		{
			printf("%x ",AckBuffer[i]);
		}
		printf("The Date :  Y:20%0.2d - M:%0.2d - D:%0.2d - W:%0.2d\r\n", rtc_datestructure.RTC_Year,rtc_datestructure.RTC_Month, rtc_datestructure.RTC_Date,rtc_datestructure.RTC_WeekDay);
		printf("The Time :  %0.2d:%0.2d:%0.2d \r\n\r\n", rtc_timestructure.RTC_Hours, rtc_timestructure.RTC_Minutes, rtc_timestructure.RTC_Seconds);
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

/*===========================================================================
* 函数 ：GetNandFlashAddr() => 获得NandFlash地址							                              																*
============================================================================*/
void GetNandFlashAddr(flashInfo* pFlshInfo)
{
	pFlshInfo->blockNo = 10;
	pFlshInfo->pageNo = 0;
	pFlshInfo->pageAddr = 0;
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
