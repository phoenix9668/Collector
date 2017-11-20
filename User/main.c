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
#include	<stdlib.h>


/** @addtogroup STM32F4_Discovery_Peripheral_Examples
  * @{
  */

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
#define TX              1       // 发送模式
#define RX              0       // 接收模式
#define ACK_LENGTH      10      // 应答信号长度      
#define SEND_LENGTH     60      // 发送数据每包的长度
#define RECV_TIMEOUT    800     // 接收超时

extern uint8_t   SendFlag;      // =1发送无线数据，=0不处理
extern uint16_t  RecvWaitTime;  // 接收等待时间
uint8_t	Chip_Addr	= 0;					// 芯片地址
uint16_t SendCnt = 0;           // 计数发送的数据包数  
uint16_t RecvCnt = 0;           // 计数接收的数据包数

// 需要发送的数据  
uint8_t SendBuffer[SEND_LENGTH] = {0,0,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','d','a','l','u','o','b'}; 
// 需要应答的数据
uint8_t AckBuffer[ACK_LENGTH] = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19};

uint8_t temp2;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nCount);
static void Show_Message(void);
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
	CC1101Init(addr, sync);                       		// 初始化CC1101寄存器
	if (RX == mode)     {CC1101SetTRMode(RX_MODE);}			// 接收模式
	else                {CC1101SetTRMode(TX_MODE);}   // 发送模式
}

/*===========================================================================
* 函数: System_Initial() => 初始化系统所有外设                              *
============================================================================*/
void System_Initial(void)
{
    MCU_Initial();      // 初始化CPU所有硬件
    RF_Initial(0x5, 0xD391, RX);     // 初始化无线芯片,发送模式       
}

/*===========================================================================
* 函数 : BSP_RF_SendPacket() => 无线发送数据函数                            *
* 输入 ：Sendbuffer指向待发送的数据，length发送数据长度                     *
* 输出 ：0，发送失败；else，发送成功                                        *
============================================================================*/
uint8_t RF_SendPacket(void)
{

    uint8_t i=0;
    uint8_t SendBuffer[64];
		int chip_address;
//    uint8_t length;
    // 数组请零
    for (i=0; i<SEND_LENGTH; i++)
        {SendBuffer[i] = 0;}
		printf("set receive chip address in package\r\n");
    scanf("%d",&chip_address);
		printf("%x\n", chip_address);
		getchar();																// 排除回车
		RF_Initial(chip_address, 0xD391, RX);     // 初始化无线芯片  				
				
		printf("please write down what you want to say\r\n");
    scanf("%[^\n]",SendBuffer);
//    length=strlen(temp);
    printf("%s\n", SendBuffer);
//    printf("length = %d\r\n", length);

    CC1101SendPacket(SendBuffer, SEND_LENGTH, ADDRESS_CHECK);    // 发送数据
    
    Usart_SendString(DEBUG_USART,"Transmit OK\r\n");    
    
    CC1101SetTRMode(RX_MODE);       // 进入接收模式，等待应答
    
    Delay(0x3FFFFF);
    
    //i=CC1101ReadStatus(CC1101_TXBYTES);//for test, TX status
    
    return(1);  
}

/*===========================================================================
* 函数 ：RF_RecvHandler() => 无线数据接收处理                               * 
============================================================================*/
void RF_RecvHandler(void)
{
    uint8_t i=0, length=0, recv_buffer[SEND_LENGTH]={0};
    
    CC1101ReadStatus( CC1101_RXBYTES );//for test, TX status
    //temp2 = GPIO_ReadInputDataBit(CC1101_IRQ_GPIO_PORT,CC1101_IRQ_PIN);
    //printf("%d\n",temp2);
    CC1101SetTRMode(RX_MODE);           // 设置RF芯片接收模式，接收数据
    
    if (0 == CC1101_IRQ_READ())         // 检测无线模块是否产生接收中断 
    {
        printf("interrupt occur\n");
        //temp2 = GPIO_ReadInputDataBit(CC1101_IRQ_GPIO_PORT,CC1101_IRQ_PIN);
        //printf("%d\n",temp2);
        while (CC1101_IRQ_READ() == 0);

        // 数据请零，防止误判断
        for (i=0; i<SEND_LENGTH; i++)   { recv_buffer[i] = 0; } 
            
        // 读取接收到的数据长度和数据内容
        length = CC1101RecPacket(recv_buffer, &Chip_Addr);
        // 打印数据
				if(length == 0)
				{
					printf("receive error or Address Filtering fail\n");
				}
				else
				{
					printf("receive data length is %d\n",length);
					printf("receive chip address is %d\n",Chip_Addr);
					printf("receive data is %s\n",recv_buffer);
				}

    }    
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
        if( Key_Scan(USER_KEY_GPIO_PORT, USER_KEY_PIN) == KEY_ON  )
		{
            printf("start transfer!!\r\n");
            LED4_Green_ON();
            LED3_Orange_OFF();
            RF_SendPacket();    // 数据发送函数
		}
        else
        {
            LED3_Orange_ON();
            LED4_Green_OFF();
            RF_RecvHandler();   // 无线数据接收处理  
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
