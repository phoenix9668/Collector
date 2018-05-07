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
RTC_DateTypeDef	rtc_datestructure;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t	Chip_Addr	= 0;								// cc1101地址
uint8_t	RSSI = 0;											// RSSI值

wiz_NetInfo gWIZNETINFO = {{0x00,0x08,0xDC,0x00,0xAB,0xCD},///< Source Mac Address
													{0xC0,0xA8,0x1,0x7B},///< Source IP Address
													{0xFF,0xFF,0xFF,0x0},///< Subnet Mask
													{0xC0,0xA8,0x1,0x1},///< Gateway IP Address
													{0,0,0,0},///< DNS server IP Address
													NETINFO_DHCP};///< 1 - Static, 2 - DHCP
uint8_t buffer[2048];											//用于存放socket的通信数据
uint8_t str3[22] = {'n','o',' ','t','h','i','s',' ','f','u','n','c','t','i','o','n',' ','n','o','w',0x0D,0x0A};
uint8_t str4[22] = {'f','u','n','c','t','i','o','n',' ','o','r','d','e','r',' ','e','r','r','o','r',0x0D,0x0A};
uint8_t str5[21] = {'d','e','v','i','c','e',' ','n','u','m','b','e','r',' ','e','r','r','o','r',0x0D,0x0A};
uint8_t str6[19] = {'R','F','I','D',' ','c','o','d','i','n','g',' ','e','r','r','o','r',0x0D,0x0A};
uint8_t str7[41] = {'r','e','c','e','i','v','e',' ','e','r','r','o','r',' ','o','r',' ','A','d','d','r','e','s','s',' ','F','i','l','t','e','r','i','n','g',' ','f','a','i','l',0x0D,0x0A};
uint8_t str8[38] = {'R','F','I','D',' ','r','e','c','e','i','v','e',' ','p','a','c','k','a','g','e',' ','b','e','g','i','n','n','i','n','g',' ','e','r','r','o','r',0x0D,0x0A};	
uint8_t str9[35] = {'R','F','I','D',' ','r','e','c','e','i','v','e',' ','f','u','n','c','t','i','o','n',' ','o','r','d','e','r',' ','e','r','r','o','r',0x0D,0x0A};	
uint8_t str10[24] = {'R','F','I','D',' ','r','e','c','e','i','v','e',' ','c','r','c',' ','e','r','r','o','r',0x0D,0x0A};
	
flashInfo flshInfo;
NAND_IDTypeDef NAND_ID;
uint8_t	gBuff[300] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
uint8_t	buff[300] = {0};
static const uint8_t RFID_init[RFID_SUM][7]= 
{
  {0x35, 0x4D, 0x5A, 0x00, 0x00, 0x00, 0x01},
  {0x36, 0x6D, 0x7A, 0x00, 0x00, 0x00, 0x02},
  {0x37, 0x12, 0x34, 0x00, 0x00, 0x00, 0x03},
  {0x38, 0x23, 0x45, 0x00, 0x00, 0x00, 0x04},
  {0x39, 0x34, 0x56, 0x00, 0x00, 0x00, 0x05},
  {0x40, 0x45, 0x67, 0x00, 0x00, 0x00, 0x06},
  {0x41, 0x56, 0x78, 0x00, 0x00, 0x00, 0x07},
  {0x42, 0x67, 0x89, 0x00, 0x00, 0x00, 0x08},
  {0x43, 0x78, 0x9A, 0x00, 0x00, 0x00, 0x09},
};

extern uint8_t PCCommend[PCCOMMEND_LENGTH];	// 接收上位机命令数组
extern __IO ITStatus RFReady;
uint8_t SendBuffer[SEND_LENGTH] = {0};// 发送数据包
uint8_t RecvBuffer[RECV_LENGTH] = {0};// 接收数据包
uint8_t AckBuffer[ACK_LENGTH] = {0};	// 应答数据包
float ADC_ConvertedValueLocal[3];			// 用于保存转化计算后的电压值
/* Private function prototypes -----------------------------------------------*/
extern void Delay(__IO uint32_t nCount);
/* Private functions ---------------------------------------------------------*/

/*===========================================================================
* 函数 ：MCU_Initial() => 初始化CPU所有硬件                                	*
* 说明 ：关于所有硬件的初始化操作，已经被建成C库，见bsp.c文件              	*
============================================================================*/
void MCU_Initial(void)
{ 
    GPIO_Config();         					// 初始化GPIO
		Debug_USART_Config();  // 初始化串口
    Key_GPIO_Config();      		// 初始化按键
    TIMx_Configuration();   		// 初始化定时器6，0.5s 一个事件
    SPI_Config();          							// 初始化SPI
		RTC_Config();											// 初始化RTC
		MOD_GPRS_Config();				// 初始化GPRS
//		FSMC_GPIO_Config();			// 初始化FSMC接口
//		FSMC_NAND_Config();			// 初始化FSMC接口
    W5500_SPI_Config();				//Config SPI
    Delay(0xFFFF);
		W5500_SPI_cbfunc();
		ChipParametersConfiguration();
		NetworkParameterConfiguration();
}

/*===========================================================================
* 函数 ：RF_Initial() => 初始化RF芯片                                      	*
* 输入 ：mode, =0,接收模式， else,发送模式                                 	*
* 说明 ：CC1101的操作，已经被建成C库，见CC1101.c文件， 提供SPI和CSN操作，		*
         即可调用其内部所有函数用户无需再关心CC1101的寄存器操作问题。      	*
============================================================================*/
void RF_Initial(uint8_t addr, uint16_t sync, uint8_t mode)
{
	CC1101Init(addr, sync);                       			// 初始化CC1101寄存器
	if(mode == RX)				{CC1101SetTRMode(RX_MODE, DISABLE);}		// 接收模式
	else if(mode == TX)		{CC1101SetTRMode(TX_MODE, DISABLE);}   // 发送模式
	else
	{
		CC1101SetIdle();																	// 空闲模式，以转到sleep状态
		CC1101WORInit();																	// 初始化电磁波激活功能
		CC1101SetWORMode();
	}
}

/*===========================================================================
* 函数: System_Initial() => 初始化系统所有外设                             	*
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
* 函数 ：Function_Ctrl() => 功能控制码解析和流程分解                      	* 
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
										#ifdef ETHERNET_ENABLE
											Delay(0xffff);
											send_tcpc(0,str3,22,gWIZNETINFO.gw,5000);			
										#else
											printf("no this function now\n");
										#endif
										break;
			/* 查询标签电量、加速度过小，加速度过大标志 */
			case 0xA3A3:	
										#ifdef ETHERNET_ENABLE
											Delay(0xffff);
											send_tcpc(0,str3,22,gWIZNETINFO.gw,5000);
										#else
											printf("no this function now\n");
										#endif
										break;
			/* 上位机查询指定编号标签 */
			case 0xA4A4:	
										Check_Assign_RFID(commend);
										break;
			/* 上位机编程标签地址码、同步码、RFID码 */
			case 0xA5A5:	
										Prog_Assign_RFID(commend);
										break;
			default:			
										#ifdef ETHERNET_ENABLE
											Delay(0xffff);
											send_tcpc(0,str4,22,gWIZNETINFO.gw,5000);			
										#else
											printf("function order error\n");
										#endif
										break;
		}
	}
	else
	{
		#ifdef ETHERNET_ENABLE
			Delay(0xffff);
			send_tcpc(0,str5,21,gWIZNETINFO.gw,5000);
		#else
			printf("device number error\n");
		#endif
	}
}

/*===========================================================================
* 函数 Check_All_RFID() => 上位机查询所有编号标签                     			* 
============================================================================*/
void Check_All_RFID(uint8_t *commend)
{   	
	uint32_t rfid_init;
	uint16_t syncword;
	uint8_t i=0;
	
	for(i=0; i<RFID_SUM; i++)
	{
		syncword = (uint16_t)(0xFF00 & RFID_init[i][1]<<8)+(uint16_t)(0x00FF & RFID_init[i][2]);
		rfid_init = ((uint32_t)(0xFF000000 & RFID_init[i][3]<<24)+(uint32_t)(0x00FF0000 & RFID_init[i][4]<<16)+(uint32_t)(0x0000FF00 & RFID_init[i][5]<<8)+(uint32_t)(0x000000FF & RFID_init[i][6]));
		RF_Initial(RFID_init[i][0], syncword, RX);
		RF_SendPacket(commend, rfid_init);
		Delay(0xFFFF);
	}
}

/*===========================================================================
* 函数 ：Check_Assign_RFID() => 上位机查询指定编号标签                    	* 
============================================================================*/
void Check_Assign_RFID(uint8_t *commend)
{
	uint32_t rfid,rfid_init;
	uint16_t syncword;
	uint8_t i=0,index=0;
	
	rfid = ((uint32_t)(0xFF000000 & commend[6]<<24)+(uint32_t)(0x00FF0000 & commend[7]<<16)+(uint32_t)(0x0000FF00 & commend[8]<<8)+(uint32_t)(0x000000FF & commend[9]));
	
	for(i=0; i<RFID_SUM; i++)
	{
		rfid_init = ((uint32_t)(0xFF000000 & RFID_init[i][3]<<24)+(uint32_t)(0x00FF0000 & RFID_init[i][4]<<16)+(uint32_t)(0x0000FF00 & RFID_init[i][5]<<8)+(uint32_t)(0x000000FF & RFID_init[i][6]));
		if(rfid == rfid_init)
		{
			syncword = (uint16_t)(0xFF00 & RFID_init[i][1]<<8)+(uint16_t)(0x00FF & RFID_init[i][2]);
			RF_Initial(RFID_init[i][0], syncword, RX);
			RF_SendPacket(commend, rfid_init);
			index = 1;
		}
	}
	if(index == 0)
	{
		#ifdef ETHERNET_ENABLE
			Delay(0xffff);
			send_tcpc(0,str6,19,gWIZNETINFO.gw,5000);
		#else
			printf("RFID coding error\n");
		#endif
	}
}

/*===========================================================================
* 函数 ：Prog_Assign_RFID() => 上位机编程标签地址码、同步码、RFID码       	* 
============================================================================*/
void Prog_Assign_RFID(uint8_t *commend)
{   	
	RF_Initial(0x20, 0x2020, RX);     // 初始化无线芯片
	RF_SendPacket(commend, 0x20202020);
}

/*===========================================================================
* 函数 : RF_SendPacket() => 无线发送数据函数                            		*
* 输入 ：Sendbuffer指向待发送的数据，length发送数据长度                    	*
* 输出 ：0，发送失败；else，发送成功                                        *
============================================================================*/
void RF_SendPacket(uint8_t *commend, uint32_t rfid)
{
	uint8_t i=0;
	
	TIM_ITConfig(BASIC_TIM,TIM_IT_Update,DISABLE);// 关闭定时器中断
	
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
	else if(commend[4] == 0xA5 && commend[5] == 0xA5)
	{
		SendBuffer[0] = 0xAB;
		SendBuffer[1] = 0xCD;
		SendBuffer[2] = commend[2];
		SendBuffer[3] = commend[3];
		SendBuffer[4] = 0xC5;
		SendBuffer[5] = 0xC5;
		for (i=6; i<SEND_LENGTH; i++) // clear array
		{SendBuffer[i] = commend[i];}
	}

	for(i=0; i<SEND_PACKAGE_NUM; i++)
	{
		CC1101SendPacket(SendBuffer, SEND_LENGTH, ADDRESS_CHECK);    // 发送数据
		Delay(0xFFFF);									// 计算得到平均27ms发送一次数据
//		Delay(0xFFFFF);								// 计算得到平均130ms发送一次数据
	}
	CC1101SetTRMode(RX_MODE, ENABLE);       	// 进入接收模式，等待应答
	Usart_SendString(DEBUG_USART,"Transmit OK\n");
	TIM_ITConfig(BASIC_TIM,TIM_IT_Update,ENABLE);	// 开启定时器中断
//	RF_Initial(addr, sync, RX);
	RecvWaitTime = RECV_TIMEOUT;
	while((RF_Acknowledge() == 0 || RF_Acknowledge() == 1) && RecvFlag == 0);
	RecvWaitTime = 0;
	RecvFlag == 0;
}

/*===========================================================================
* 函数 ：RF_Acknowledge() => 无线数据接收应答                              	*
============================================================================*/
uint8_t	RF_Acknowledge(void)
{
	uint8_t i=0, length=0;
	uint8_t index;
	int16_t rssi_dBm;	
	
	if(RFReady == SET)         // 检测无线模块是否产生接收中断 
	{
		/* Reset transmission flag */
		RFReady = RESET;
		/* clear array */
		for (i=0; i<RECV_LENGTH; i++)   { RecvBuffer[i] = 0; } 
		length = CC1101RecPacket(RecvBuffer, &Chip_Addr, &RSSI);// 读取接收到的数据长度和数据内容
		/* Set the CC1101_IRQ_EXTI_LINE enable */
		EXTI_Config(CC1101_IRQ_EXTI_LINE, EXTI_Trigger_Falling, ENABLE);
		// 打印数据
		rssi_dBm = CC1101CalcRSSI_dBm(RSSI);
		printf("RSSI = %ddBm, length = %d, address = %d\n",rssi_dBm,length,Chip_Addr);
		rssi_dBm = CC1101CalcRSSI_dBm(RecvBuffer[18]);
		printf("RFID RSSI = %ddBm\n",rssi_dBm);
		for(i=0; i<RECV_LENGTH; i++)
		{
			printf("%x ",RecvBuffer[i]);
		}
		printf("\r\n");		
		if(length == 0)
		{
			index = 1;
			printf("index = %d\n",index);
			Reply_PC(index);
			return 1;
		}
		else if(length == 1)
		{
			index = 12;
			printf("index = %d\n",index);
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
						printf("index = %d\n",index);
						Reply_PC(index);
						return 4;
					}
					else if(RecvBuffer[4] == 0xD3 && RecvBuffer[5] == 0x01)
					{
						index = 5;
						printf("index = %d\n",index);
						Reply_PC(index);
						return 5;
					}
					else if(RecvBuffer[4] == 0xD4 && RecvBuffer[5] == 0x01)
					{
						index = 6;
						printf("index = %d\n",index);
						Reply_PC(index);
						return 6;
					}
					else if(RecvBuffer[4] == 0xD5 && RecvBuffer[5] == 0x01)
					{
						index = 7;
						printf("index = %d\n",index);
						Reply_PC(index);
						return 7;
					}
					else if(RecvBuffer[4] == 0x02 && RecvBuffer[5] == 0x02)
					{
						index = 8;
						printf("index = %d\n",index);
						Reply_PC(index);
						return 8;
					}
					else if(RecvBuffer[4] == 0x03 && RecvBuffer[5] == 0x03)
					{
						index = 9;
						printf("index = %d\n",index);
						Reply_PC(index);
						return 9;
					}
					else
					{	
						index = 3;
						printf("index = %d\n",index);
						Reply_PC(index);
						return 3;
					}
				}
				else
				{	
					index = 2;
					printf("index = %d\n",index);
					Reply_PC(index);
					return 2;
				}
			}
	}
	else	{	return 0;}
}

/*===========================================================================
* 函数 ：Reply_PC() => WIFI回复PC							                             	*
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
		AckBuffer[19] = RSSI;
		AckBuffer[20] = rtc_datestructure.RTC_Year;
		AckBuffer[21] = rtc_datestructure.RTC_Month;
		AckBuffer[22] = rtc_datestructure.RTC_Date;
		AckBuffer[23] = rtc_datestructure.RTC_WeekDay;
		AckBuffer[24] = rtc_timestructure.RTC_Hours;
		AckBuffer[25] = rtc_timestructure.RTC_Minutes;
		AckBuffer[26] = rtc_timestructure.RTC_Seconds;
		#ifdef ETHERNET_ENABLE
			Delay(0xffff);
			send_tcpc(0,AckBuffer,ACK_LENGTH,gWIZNETINFO.gw,5000);
		#else
			for(i=0; i<ACK_LENGTH; i++)
			{
				printf("%x ",AckBuffer[i]);
			}
			printf("\n");
		#endif
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
		AckBuffer[19] = RSSI;
		AckBuffer[20] = rtc_datestructure.RTC_Year;
		AckBuffer[21] = rtc_datestructure.RTC_Month;
		AckBuffer[22] = rtc_datestructure.RTC_Date;
		AckBuffer[23] = rtc_datestructure.RTC_WeekDay;
		AckBuffer[24] = rtc_timestructure.RTC_Hours;
		AckBuffer[25] = rtc_timestructure.RTC_Minutes;
		AckBuffer[26] = rtc_timestructure.RTC_Seconds;
		#ifdef ETHERNET_ENABLE
			Delay(0xffff);
			send_tcpc(0,AckBuffer,ACK_LENGTH,gWIZNETINFO.gw,5000);
		#else
			for(i=0; i<ACK_LENGTH; i++)
			{
				printf("%x ",AckBuffer[i]);
			}
			printf("\n");
		#endif
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
		AckBuffer[19] = RSSI;
		AckBuffer[20] = rtc_datestructure.RTC_Year;
		AckBuffer[21] = rtc_datestructure.RTC_Month;
		AckBuffer[22] = rtc_datestructure.RTC_Date;
		AckBuffer[23] = rtc_datestructure.RTC_WeekDay;
		AckBuffer[24] = rtc_timestructure.RTC_Hours;
		AckBuffer[25] = rtc_timestructure.RTC_Minutes;
		AckBuffer[26] = rtc_timestructure.RTC_Seconds;
		#ifdef ETHERNET_ENABLE
			Delay(0xffff);
			send_tcpc(0,AckBuffer,ACK_LENGTH,gWIZNETINFO.gw,5000);
		#else
			for(i=0; i<ACK_LENGTH; i++)
			{
				printf("%x ",AckBuffer[i]);
			}
			printf("\n");
		#endif
	}	
	else if(index == 7)
	{
		RTC_TimeAndDate_Access(&rtc_timestructure, &rtc_datestructure);
		AckBuffer[0] = 0xAB;
		AckBuffer[1] = 0xCD;
		AckBuffer[2] = RecvBuffer[2];
		AckBuffer[3] = RecvBuffer[3];
		AckBuffer[4] = 0xB5;
		AckBuffer[5] = 0x01;
		for(i=0;i<ACK_LENGTH-14;i++)
		{
			AckBuffer[i+6] = RecvBuffer[i+6];
		}
		AckBuffer[19] = RSSI;
		AckBuffer[20] = rtc_datestructure.RTC_Year;
		AckBuffer[21] = rtc_datestructure.RTC_Month;
		AckBuffer[22] = rtc_datestructure.RTC_Date;
		AckBuffer[23] = rtc_datestructure.RTC_WeekDay;
		AckBuffer[24] = rtc_timestructure.RTC_Hours;
		AckBuffer[25] = rtc_timestructure.RTC_Minutes;
		AckBuffer[26] = rtc_timestructure.RTC_Seconds;
		#ifdef ETHERNET_ENABLE
			Delay(0xffff);
			send_tcpc(0,AckBuffer,ACK_LENGTH,gWIZNETINFO.gw,5000);
		#else
			for(i=0; i<ACK_LENGTH; i++)
			{
				printf("%x ",AckBuffer[i]);
			}
			printf("\n");
		#endif
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
		AckBuffer[19] = RSSI;
		AckBuffer[20] = rtc_datestructure.RTC_Year;
		AckBuffer[21] = rtc_datestructure.RTC_Month;
		AckBuffer[22] = rtc_datestructure.RTC_Date;
		AckBuffer[23] = rtc_datestructure.RTC_WeekDay;
		AckBuffer[24] = rtc_timestructure.RTC_Hours;
		AckBuffer[25] = rtc_timestructure.RTC_Minutes;
		AckBuffer[26] = rtc_timestructure.RTC_Seconds;
		#ifdef ETHERNET_ENABLE
			Delay(0xffff);
			send_tcpc(0,AckBuffer,ACK_LENGTH,gWIZNETINFO.gw,5000);
		#else
			for(i=0; i<ACK_LENGTH; i++)
			{
				printf("%x ",AckBuffer[i]);
			}
			printf("\n");
		#endif
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
		AckBuffer[19] = RSSI;
		AckBuffer[20] = rtc_datestructure.RTC_Year;
		AckBuffer[21] = rtc_datestructure.RTC_Month;
		AckBuffer[22] = rtc_datestructure.RTC_Date;
		AckBuffer[23] = rtc_datestructure.RTC_WeekDay;
		AckBuffer[24] = rtc_timestructure.RTC_Hours;
		AckBuffer[25] = rtc_timestructure.RTC_Minutes;
		AckBuffer[26] = rtc_timestructure.RTC_Seconds;
		#ifdef ETHERNET_ENABLE
			Delay(0xffff);
			send_tcpc(0,AckBuffer,ACK_LENGTH,gWIZNETINFO.gw,5000);
		#else
			for(i=0; i<ACK_LENGTH; i++)
			{
				printf("%x ",AckBuffer[i]);
			}
			printf("\n");
		#endif
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
		for(i=0;i<10;i++)
		{
			AckBuffer[i+10] = 0;
		}
		AckBuffer[20] = rtc_datestructure.RTC_Year;
		AckBuffer[21] = rtc_datestructure.RTC_Month;
		AckBuffer[22] = rtc_datestructure.RTC_Date;
		AckBuffer[23] = rtc_datestructure.RTC_WeekDay;
		AckBuffer[24] = rtc_timestructure.RTC_Hours;
		AckBuffer[25] = rtc_timestructure.RTC_Minutes;
		AckBuffer[26] = rtc_timestructure.RTC_Seconds;
		#ifdef ETHERNET_ENABLE
			Delay(0xffff);
			send_tcpc(0,AckBuffer,ACK_LENGTH,gWIZNETINFO.gw,5000);
		#else
			for(i=0; i<ACK_LENGTH; i++)
			{
				printf("%x ",AckBuffer[i]);
			}
			printf("\n");
		#endif
		printf("The Date :  Y:20%0.2d - M:%0.2d - D:%0.2d - W:%0.2d\r\n", rtc_datestructure.RTC_Year,rtc_datestructure.RTC_Month, rtc_datestructure.RTC_Date,rtc_datestructure.RTC_WeekDay);
		printf("The Time :  %0.2d:%0.2d:%0.2d \r\n\r\n", rtc_timestructure.RTC_Hours, rtc_timestructure.RTC_Minutes, rtc_timestructure.RTC_Seconds);
	}
	else if(index == 1)
	{
		#ifdef ETHERNET_ENABLE
			Delay(0xffff);
			send_tcpc(0,str7,41,gWIZNETINFO.gw,5000);
		#else
			printf("receive error or Address Filtering fail\n");
		#endif
	}
	else if(index == 2)
	{
		#ifdef ETHERNET_ENABLE
			Delay(0xffff);
			send_tcpc(0,str8,38,gWIZNETINFO.gw,5000);
		#else
			printf("RFID receive package beginning error\r\n");
		#endif
	}
	else if(index == 3)
	{
		#ifdef ETHERNET_ENABLE
			Delay(0xffff);
			send_tcpc(0,str9,35,gWIZNETINFO.gw,5000);
		#else
			printf("RFID receive function order error\r\n");
		#endif
	}
	else if(index == 12)
	{
		#ifdef ETHERNET_ENABLE
			Delay(0xffff);
			send_tcpc(0,str10,24,gWIZNETINFO.gw,5000);
		#else
			printf("RFID receive crc error\r\n");
		#endif
		
	}
}

/*===========================================================================
* 函数 ：GetNandFlashAddr() => 获得NandFlash地址							            	*
============================================================================*/
void GetNandFlashAddr(flashInfo* pFlshInfo)
{
	pFlshInfo->blockNo = 10;
	pFlshInfo->pageNo = 0;
	pFlshInfo->pageAddr = 0;
}


/*===========================================================================
* 函数 ：network_init() => 以太网初始化							            						*
============================================================================*/

//初始化芯片参数
void ChipParametersConfiguration(void)
{
  uint8_t tmp;
  uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};
  //WIZCHIP SOCKET缓存区初始化
  if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1){
		#ifdef ETHERNET_ENABLE
		printf("WIZCHIP Initialized fail.\r\n");
		#endif
		while(1);
  }

  //PHY物理层连接状态检查
  do{
    if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1){
			#ifdef ETHERNET_ENABLE
			printf("Unknown PHY Link stauts.\r\n");
			#endif	
    }
  }while(tmp == PHY_LINK_OFF);
}

/**
  * @brief  Intialize the network information to be used in WIZCHIP
  * @retval None
  */
void NetworkParameterConfiguration(void)
{
	uint8_t tmpstr[6];
	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
	ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);
 
	// Display Network Information
	ctlwizchip(CW_GET_ID,(void*)tmpstr);
	#ifdef ETHERNET_ENABLE
	printf("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
	printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],
          gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
	printf("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
	printf("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
	printf("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
	printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
	printf("======================\r\n");
	#endif
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
