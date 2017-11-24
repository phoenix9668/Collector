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
#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./tim/bsp_basic_tim.h"
#include "./spi/bsp_spi.h"
#include "./key/bsp_key.h" 
#include "stm32f4_discovery.h"


/** @addtogroup STM32F4_Discovery_Peripheral_Examples
  * @{
  */

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
#define TX              0       // 发送模式
#define RX              1       // 接收模式
#define IDLE          	2       // 空闲模式
#define ACK_LENGTH      11      // 应答信号长度      
#define SEND_LENGTH     60      // 发送数据每包的长度
#define RECV_TIMEOUT		2000    // 接收等待2s

extern uint16_t RecvWaitTime;  	// 接收等待时间
extern uint8_t RecvFlag;       	// =1接收等待时间结束，=0不处理
uint8_t	Chip_Addr	= 0;					// 芯片地址
uint8_t	RSSI = 0;								// RSSI状态值
uint16_t SendCnt = 0;           // 计数发送的数据包数
uint16_t RecvCnt = 0;           // 计数接收的数据包数

// 需要发送的数据  
uint8_t SendBuffer[SEND_LENGTH] = {0};
// 需要应答的数据
uint8_t AckBuffer[ACK_LENGTH] = {'a','c','k','n','o','w','l','e','d','g','e'};

uint8_t temp2;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nCount);
static void Show_Message(void);
uint8_t	RF_Acknowledge(void);
/* Private functions ---------------------------------------------------------*/

/*===========================================================================
* 函数 ：MCU_Initial() => 初始化CPU所有硬件                                 *
* 说明 ：关于所有硬件的初始化操作，已经被建成C库，见bsp.c文件               *
============================================================================*/
void MCU_Initial(void)
{ 
    Debug_USART_Config();   // 初始化串口
    GPIO_Initial();         // 初始化GPIO
    Key_GPIO_Config();      // 初始化按键
    TIMx_Configuration();   // 初始化定时器6，0.5s 一个事件
    SPI_Initial();          // 初始化SPI                           
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
    RF_Initial(0x5, 0xD391, IDLE);     // 初始化无线芯片,空闲模式       
}

/*===========================================================================
* 函数 : RF_SendPacket() => 无线发送数据函数                            *
* 输入 ：Sendbuffer指向待发送的数据，length发送数据长度                     *
* 输出 ：0，发送失败；else，发送成功                                        *
============================================================================*/
uint8_t RF_SendPacket(void)
{

	uint8_t i=0;	
	
	for (i=0; i<SEND_LENGTH; i++) // clear array
		{SendBuffer[i] = 0;}	
	printf("please write down what you want to say\r\n");
	scanf("%[^\n]",SendBuffer);
	printf("%s\n", SendBuffer);
		
	for(i=0; i<100; i++)
	{
		SendBuffer[0] = i;
		CC1101SendPacket(SendBuffer, SEND_LENGTH, ADDRESS_CHECK);    // 发送数据

		CC1101SetTRMode(RX_MODE);       // 进入接收模式，等待应答
		
		Delay(0xFFFF);									// 计算得到平均27ms发送一次数据
//		Delay(0xFFFFF);									// 计算得到平均130ms发送一次数据
	}

	Usart_SendString(DEBUG_USART,"Transmit OK\r\n");
	RF_Initial(0x5, 0xD391, RX);
	RecvWaitTime = RECV_TIMEOUT;
	while(RF_Acknowledge() == 0 && RecvFlag == 0);
	RecvWaitTime = 0;
	CC1101SetIdle();																	// 空闲模式，以转到sleep状态
	CC1101WORInit();																	// 初始化电磁波激活功能
	CC1101SetWORMode();
	//i=CC1101ReadStatus(CC1101_TXBYTES);//for test, TX status
	return(1);
}


/*===========================================================================
* 函数 ：Get_Address() => 设置数据包地址和同步                             * 
============================================================================*/
void Get_Address(void)
{   
		int chip_address;
		unsigned int	sync_word;
	
		CC1101SetIdle();			//退出WOR模式
		Delay(0x3FFFF);
//		temp2 = CC1101ReadStatus(CC1101_MARCSTATE);//for test, right is IDLE status
//		printf("state is %x\n",temp2);
	
		printf("set receive chip address in package\r\n");
    scanf("%d",&chip_address);
		printf("%x\n", chip_address);
		getchar();																// 排除回车
	
		printf("set receive chip sync word in package\r\n");
    scanf("%x",&sync_word);
		printf("%x\n", sync_word);
		getchar();																// 排除回车
		RF_Initial(chip_address, sync_word, RX);     // 初始化无线芯片
}

/*===========================================================================
* 函数 ：RF_Acknowledge() => 无线数据接收应答                               * 
============================================================================*/
uint8_t	RF_Acknowledge(void)
{
	uint8_t i=0, k, length=0, rec_buffer[ACK_LENGTH]={0};
	int16_t rssi_dBm;

//	CC1101SetTRMode(RX_MODE);           // 设置RF芯片接收模式，接收数据
//	temp2 = CC1101ReadStatus(CC1101_MARCSTATE);//for test, TX status
//	printf("state is %x\n",temp2);
	
	if(CC1101_IRQ_READ() == 0)         // 检测无线模块是否产生接收中断 
	{
		while (CC1101_IRQ_READ() == 0);
		for (i=0; i<ACK_LENGTH; i++)   { rec_buffer[i] = 0; } // clear array

		// 读取接收到的数据长度和数据内容
		length = CC1101RecPacket(rec_buffer, &Chip_Addr, &RSSI);
		// 打印数据
		if(length == 0)
		{
			printf("receive error or Address Filtering fail\n");
			return 0;
		}
		else
		{
			rssi_dBm = CC1101CalcRSSI_dBm(RSSI);
			printf("RSSI = %ddBm, length = %d, address = %d\n",rssi_dBm,length,Chip_Addr);
			for(k=0; k<ACK_LENGTH; k++)
			{	
				printf("%c",rec_buffer[k]);
			}
			printf("\n");
			return 1;
		}
	}
	else	{return 0;}
}
/*===========================================================================
* 函数 ：RF_RecvHandler() => 无线数据接收处理                               * 
============================================================================*/
uint8_t RF_RecvHandler(void)
{
	uint8_t i=0, length=0, recv_buffer[SEND_LENGTH]={0};
	int16_t rssi_dBm;
	//CC1101ReadStatus(CC1101_RXBYTES);//for test, TX status

	//CC1101SetTRMode(RX_MODE);           // 设置RF芯片接收模式，接收数据
	
	if(CC1101_GDO2_READ() == 0)
	{
		while (CC1101_GDO2_READ() == 0);
		LED5_Red_TOG();
	}
	
	if(CC1101_IRQ_READ() == 0)         // 检测无线模块是否产生接收中断 
		{
			printf("interrupt occur\n");
			while (CC1101_IRQ_READ() == 0);
			for (i=0; i<SEND_LENGTH; i++)   { recv_buffer[i] = 0; } // clear array
            
//			rssi_p = CC1101ReadRSSI();
//			printf("RSSI_p = %ddBm ",rssi_p);
			// 读取接收到的数据长度和数据内容
			length = CC1101RecPacket(recv_buffer, &Chip_Addr, &RSSI);
			// 打印数据
			if(length == 0)
				{
					printf("receive error or Address Filtering fail\n");
					return 0;
				}
			else
				{
					rssi_dBm = CC1101CalcRSSI_dBm(RSSI);
					printf("RSSI = %ddBm, length = %d, address = %d, number = %d\n%s\n",rssi_dBm,length,Chip_Addr,recv_buffer[0],recv_buffer);
					return 1;
				}

//			temp2 = CC1101ReadStatus(CC1101_WORTIME1);//for test, TX status
//			printf("WORTIME1 is %x\n",temp2);
//			temp2 = CC1101ReadStatus(CC1101_WORTIME0);//for test, TX status
//			printf("WORTIME0 is %x\n",temp2);
		}
	else	{return 0;}
}

/*===========================================================================
* 函数 ：RF_Reply() => 无线数据接收端回复                              * 
============================================================================*/
void RF_Reply(void)
{
			uint8_t i;
			for(i=0; i<20; i++)
			{
				Delay(0xFFFFF);
				CC1101SendPacket(AckBuffer, ACK_LENGTH, ADDRESS_CHECK);    // 发送数据
			}
			CC1101SetIdle();																	// 空闲模式，以转到sleep状态
			CC1101WORInit();																	// 初始化电磁波激活功能
			CC1101SetWORMode();
}

/*===========================================================================
* 函数 : main() => 主函数，程序入口                                         *
* 说明 ：每1s发送一包数据，每包数据长度为10个字节，数据内容为0-9            *
         接收方反馈(应答)的数据长度为10个字节，数据内容为10-19              *
============================================================================*/
int main(void)
{
	System_Initial();
    
//    temp2 = CC1101ReadReg(CC1101_IOCFG0);//for test, TX status
//    printf("发送状态确认：%x\n",temp2);
	Show_Message();
	
	while(1)
		{
			//当按USER键时发送数据
			if(Key_Scan(USER_KEY_GPIO_PORT, USER_KEY_PIN) == KEY_ON)
				{
					printf("start transfer!!\r\n");
					LED4_Green_ON();
					LED3_Orange_OFF();
					Get_Address();
					RF_SendPacket();    // 数据发送函数
				}
			else
        {
					LED3_Orange_ON();
					LED4_Green_OFF();
					if(RF_RecvHandler() == 1)   // 无线数据接收处理
						{
							printf("receive succeed!!\r\n");
							RF_Reply();
						}
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
	printf(" you need press USER button when you want transfer data\r\n");
	printf(" if choose transfer,the data must not exceed 60 bytes!!\r\n");
	printf(" PS: green led light when system in transfer mode\r\n");    
	printf("     orange led light when system in receive mode\r\n");
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
