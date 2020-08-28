/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cc1101.h"
#include "fram.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern uint8_t RxBuffer[RXBUFFERSIZE];
extern __IO FlagStatus CommandState;
extern __IO ITStatus RFReady;

extern uint8_t MBID_byte1;
extern uint8_t MBID_byte2;
extern uint8_t RFID_init[RFID_SUM][7]; 
extern uint8_t FRAM_Data[FRAM_DATA_LENGTH];

uint8_t SendBuffer[SEND_LENGTH] = {0};
uint8_t RecvBuffer[RECV_LENGTH] = {0};
uint8_t AckBuffer[ACK_LLENGTH] = {0};
__IO uint32_t RecvWaitTime = 0;
uint8_t flag;
uint8_t	Chip_Addr	= 0;
uint8_t	RSSI = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI2_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	MOD_GPRS_RESET();
	Activate_SPI();
	Activate_USART3_RXIT();
	Init_ID_Info();
	Show_Message();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		if(CommandState == SET)
		{
			if(RxBuffer[0] == 0xAB && RxBuffer[1] == 0xCD)
			{
				printf("start transfer\n");
				LED_STA_ON();
				Function_Ctrl(RxBuffer);
				LED_STA_OFF();
			}
			else if(RxBuffer[0] == 0xE5 && RxBuffer[1] == 0x5E)
			{
				printf("fram transfer\n");
				LED_COM_ON();
				FRAM_Ctrl(RxBuffer);
				LED_COM_OFF();
			}
			else
			{
				LED_STA_OFF();
			}
			CommandState = RESET;
		}
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void Show_Message(void)
{
	uint16_t MBID;
	
	printf("\r\nmain program running\n");
	printf("using USART3,configuration:%d 8-N-1\n",115200);
	MBID = (uint16_t)(0xFF00 & MBID_byte1<<8)+(uint16_t)(0x00FF & MBID_byte2);
	printf("Main_Board ID:%x\n",MBID);
}

/**
  * @brief  Function_Ctrl(uint8_t *command).
  * @retval None
  */
void Function_Ctrl(uint8_t *command)
{
	if(command[2] == MBID_byte1 && command[3] == MBID_byte2)
	{
		switch(((uint16_t)(0xFF00 & command[4]<<8)+(uint16_t)(0x00FF & command[5])))
		{
			/*check all rfid*/
			case 0xA0A0:	
										Check_All_RFID(command);
										break;
			/*configure time information*/
			case 0xA101:	
//										RTC_TimeAndDate_Reset(command[10],  command[11],  command[12],  command[13],  command[14],  command[15],  command[16]);
//										RTC_Config_Check();      // ??RTC,??????????
										Reply_PC(0x0B);
										break;
			/*read time information*/
			case 0xA102:
										Reply_PC(0x0B);
										break;
			case 0xA2A2:
										Cfg_Assign_RFID(command);
										break;
			/*configure assign RFID's ADXL362 parameter*/
			case 0xA3A3:
										Cfg_Assign_RFID(command);
										break;
			/*check assign rfid*/
			case 0xA4A4:
										Check_Assign_RFID(command);
										break;
			/*check assign section rfid*/
			case 0xA401:
										Check_Assign_Section_RFID(command);
										break;
			/*configure RFID addr sync unicode*/
			case 0xA5A5:
										Prog_Assign_RFID(command);
										break;
			/*clear assign RFID data*/
			case 0xA6A6:
										Clear_Assign_RFID(command);
			case 0xA601:
										Clear_Assign_Section_RFID(command);
										break;			
			default:			
										printf("function order error\n");
										break;
		}
	}
	else
	{
		printf("device number error\n");
	}
}

/*===========================================================================
* Check_All_RFID() => check all rfid                     										* 
============================================================================*/
void Check_All_RFID(uint8_t *command)
{
	uint16_t syncword;
	uint8_t i=0;
	
	for(i=0; i<RFID_SUM; i++)
	{
		syncword = (uint16_t)(0xFF00 & RFID_init[i][1]<<8)+(uint16_t)(0x00FF & RFID_init[i][2]);
		command[6] = RFID_init[i][3];
		command[7] = RFID_init[i][4];
		command[8] = RFID_init[i][5];
		command[9] = RFID_init[i][6];
		RFIDInitial(RFID_init[i][0], syncword, RX_MODE);
		RF_SendPacket(command);
		HAL_Delay(10);
		printf("scaning:%d\n",i);
	}
	printf("All Finish\n");
}

/*===========================================================================
* Cfg_Assign_RFID() => configure assign RFID's ADXL362 parameter          	* 
============================================================================*/
void Cfg_Assign_RFID(uint8_t *command)
{
	uint32_t rfid,rfid_init;
	uint16_t syncword;
	uint8_t i=0,err=0;
	
	rfid = ((uint32_t)(0xFF000000 & command[6]<<24)+(uint32_t)(0x00FF0000 & command[7]<<16)+(uint32_t)(0x0000FF00 & command[8]<<8)+(uint32_t)(0x000000FF & command[9]));
	
	for(i=0; i<RFID_SUM; i++)
	{
		rfid_init = ((uint32_t)(0xFF000000 & RFID_init[i][3]<<24)+(uint32_t)(0x00FF0000 & RFID_init[i][4]<<16)+(uint32_t)(0x0000FF00 & RFID_init[i][5]<<8)+(uint32_t)(0x000000FF & RFID_init[i][6]));
		if(rfid == rfid_init)
		{
			syncword = (uint16_t)(0xFF00 & RFID_init[i][1]<<8)+(uint16_t)(0x00FF & RFID_init[i][2]);
			RFIDInitial(RFID_init[i][0], syncword, RX_MODE);
			RF_ProgPacket(command);
			err = 1;
		}
	}
	if(err == 0)
	{
		printf("RFID coding error\n");
	}
}

/*===========================================================================
* Check_Assign_RFID() => check assign rfid                    							* 
============================================================================*/
void Check_Assign_RFID(uint8_t *command)
{
	uint32_t rfid,rfid_init;
	uint16_t syncword;
	uint8_t i=0,err=0;
	
	rfid = ((uint32_t)(0xFF000000 & command[6]<<24)+(uint32_t)(0x00FF0000 & command[7]<<16)+(uint32_t)(0x0000FF00 & command[8]<<8)+(uint32_t)(0x000000FF & command[9]));
	
	for(i=0; i<RFID_SUM; i++)
	{
		rfid_init = ((uint32_t)(0xFF000000 & RFID_init[i][3]<<24)+(uint32_t)(0x00FF0000 & RFID_init[i][4]<<16)+(uint32_t)(0x0000FF00 & RFID_init[i][5]<<8)+(uint32_t)(0x000000FF & RFID_init[i][6]));
		if(rfid == rfid_init)
		{
			syncword = (uint16_t)(0xFF00 & RFID_init[i][1]<<8)+(uint16_t)(0x00FF & RFID_init[i][2]);
			RFIDInitial(RFID_init[i][0], syncword, RX_MODE);
			RF_SendPacket(command);
			err = 1;
		}
	}
	if(err == 0)
	{
		printf("RFID coding error\n");
	}
}

/*===========================================================================
* Check_Assign_Section_RFID() => check assign section rfid            			* 
============================================================================*/
void Check_Assign_Section_RFID(uint8_t *command)
{
	uint16_t syncword;
	uint16_t i=0;
	uint16_t j=(uint16_t)(0xFF00 & command[6]<<8)+(uint16_t)(0x00FF & command[7])-1;
	uint16_t s=(uint16_t)(0xFF00 & command[8]<<8)+(uint16_t)(0x00FF & command[9])-1;
	
	if(j<=200 && s<=200)
	{
		for(i=j; i<=s; i++)
		{
			syncword = (uint16_t)(0xFF00 & RFID_init[i][1]<<8)+(uint16_t)(0x00FF & RFID_init[i][2]);
			command[6] = RFID_init[i][3];
			command[7] = RFID_init[i][4];
			command[8] = RFID_init[i][5];
			command[9] = RFID_init[i][6];
			RFIDInitial(RFID_init[i][0], syncword, RX_MODE);
			RF_SendPacket(command);
			HAL_Delay(10);
			printf("scaning:%d\n",i);
		}
		printf("All Finish\n");
	}
	else
	{
		printf("Exceeding Threshold 200\n");
	}

}

/*===========================================================================
* Prog_Assign_RFID() => configure RFID addr sync unicode   									* 
============================================================================*/
void Prog_Assign_RFID(uint8_t *command)
{
	RFIDInitial(0x20, 0x2020, RX_MODE);
	RF_ProgPacket(command);
}

/*===========================================================================
* Clear_Assign_RFID() => clear assign RFID data                 						* 
============================================================================*/
void Clear_Assign_RFID(uint8_t *command)
{
	uint32_t rfid,rfid_init;
	uint16_t syncword;
	uint8_t i=0,err=0;
	
	rfid = ((uint32_t)(0xFF000000 & command[6]<<24)+(uint32_t)(0x00FF0000 & command[7]<<16)+(uint32_t)(0x0000FF00 & command[8]<<8)+(uint32_t)(0x000000FF & command[9]));
	
	for(i=0; i<RFID_SUM; i++)
	{
		rfid_init = ((uint32_t)(0xFF000000 & RFID_init[i][3]<<24)+(uint32_t)(0x00FF0000 & RFID_init[i][4]<<16)+(uint32_t)(0x0000FF00 & RFID_init[i][5]<<8)+(uint32_t)(0x000000FF & RFID_init[i][6]));
		if(rfid == rfid_init)
		{
			syncword = (uint16_t)(0xFF00 & RFID_init[i][1]<<8)+(uint16_t)(0x00FF & RFID_init[i][2]);
			RFIDInitial(RFID_init[i][0], syncword, RX_MODE);
			RF_SendClearPacket(command);
			err = 1;
		}
	}
	if(err == 0)
	{
		printf("RFID coding error\n");
	}
}

/*===========================================================================
* Clear_Assign_Section_RFID() => clear assign section rfid data        	* 
============================================================================*/
void Clear_Assign_Section_RFID(uint8_t *command)
{
	uint16_t syncword;
	uint16_t i=0;
	uint16_t j=(uint16_t)(0xFF00 & command[6]<<8)+(uint16_t)(0x00FF & command[7])-1;
	uint16_t s=(uint16_t)(0xFF00 & command[8]<<8)+(uint16_t)(0x00FF & command[9])-1;
	
	if(j<=200 && s<=200)
	{
		for(i=j; i<=s; i++)
		{
			syncword = (uint16_t)(0xFF00 & RFID_init[i][1]<<8)+(uint16_t)(0x00FF & RFID_init[i][2]);
			command[6] = RFID_init[i][3];
			command[7] = RFID_init[i][4];
			command[8] = RFID_init[i][5];
			command[9] = RFID_init[i][6];
			RFIDInitial(RFID_init[i][0], syncword, RX_MODE);
			RF_SendClearPacket(command);
			HAL_Delay(10);
			printf("scaning:%d\n",i);
		}
		printf("All Finish\n");
	}
	else
	{
		printf("Exceeding Threshold 200\n");
	}
}

/**
  * @brief  RF_ProgPacket(uint8_t *command).
  * @retval None
  */
void RF_ProgPacket(uint8_t *command)
{
	uint16_t i=0;
	
	for (i=0; i<SEND_LENGTH; i++) // clear array
		{SendBuffer[i] = 0;}

	if(command[4] == 0xA2 && command[5] == 0xA2)
	{	
		SendBuffer[0] = 0xAB;
		SendBuffer[1] = 0xCD;
		SendBuffer[2] = command[2];
		SendBuffer[3] = command[3];
		SendBuffer[4] = 0xC2;
		SendBuffer[5] = 0xC2;
		for (i=6; i<SEND_LENGTH; i++) // clear array
		{SendBuffer[i] = command[i];}
	}	
	else if(command[4] == 0xA3 && command[5] == 0xA3)
	{	
		SendBuffer[0] = 0xAB;
		SendBuffer[1] = 0xCD;
		SendBuffer[2] = command[2];
		SendBuffer[3] = command[3];
		SendBuffer[4] = 0xC3;
		SendBuffer[5] = 0xC3;
		for (i=6; i<SEND_LENGTH; i++) // clear array
		{SendBuffer[i] = command[i];}
	}
	else if(command[4] == 0xA5 && command[5] == 0xA5)
	{
		SendBuffer[0] = 0xAB;
		SendBuffer[1] = 0xCD;
		SendBuffer[2] = command[2];
		SendBuffer[3] = command[3];
		SendBuffer[4] = 0xC5;
		SendBuffer[5] = 0xC5;
		for (i=6; i<SEND_LENGTH; i++) // clear array
		{SendBuffer[i] = command[i];}
	}

	for(i=0; i<SEND_PACKAGE_NUM; i++)
	{
		CC1101SendPacket(SendBuffer, SEND_LENGTH, ADDRESS_CHECK);
		HAL_Delay(5);
	}
	CC1101SetTRMode(RX_MODE, ENABLE);
	printf("Transmit OK\n");
	RecvWaitTime = RECV_TIMEOUT;
	while((RF_Acknowledge() == 0x0 || RF_Acknowledge() == 0x01) && RecvWaitTime != 0)
	{
		RecvWaitTime--;
	}
	printf("Prog Complete\n");
}

/**
  * @brief  RF_SendPacket(uint8_t *command).
  * @retval None
  */
void RF_SendPacket(uint8_t *command)
{
	uint16_t i=0;
	
	for (i=0; i<SEND_LENGTH; i++) // clear array
		{SendBuffer[i] = 0;}
	
	if(command[4] == 0xA0 && command[5] == 0xA0)
	{
		SendBuffer[0] = 0xAB;
		SendBuffer[1] = 0xCD;
		SendBuffer[2] = command[2];
		SendBuffer[3] = command[3];
		SendBuffer[4] = 0xC0;
		SendBuffer[5] = 0xC0;
		SendBuffer[6] = command[6];
		SendBuffer[7] = command[7];
		SendBuffer[8] = command[8];
		SendBuffer[9] = command[9];
	}
	else if(command[4] == 0xA4 && (command[5] == 0xA4 || command[5] == 0x01))
	{
		SendBuffer[0] = 0xAB;
		SendBuffer[1] = 0xCD;
		SendBuffer[2] = command[2];
		SendBuffer[3] = command[3];
		SendBuffer[4] = 0xC4;
		SendBuffer[5] = 0xC4;
		SendBuffer[6] = command[6];
		SendBuffer[7] = command[7];
		SendBuffer[8] = command[8];
		SendBuffer[9] = command[9];
	}

	for(i=0; i<SEND_PACKAGE_NUM; i++)
	{
		CC1101SendPacket(SendBuffer, SEND_LENGTH, ADDRESS_CHECK);
		HAL_Delay(5);
	}
	CC1101SetTRMode(RX_MODE, ENABLE);
	printf("Transmit OK\n");
	RecvWaitTime = RECV_TIMEOUT;
	while((RF_Acknowledge() == 0x0 || RF_Acknowledge() == 0x01) && RecvWaitTime != 0)
	{
		RecvWaitTime--;
	}
	printf("Transmit Complete\n");
}

/**
  * @brief  RF_SendPacket(uint8_t *command).
  * @retval None
  */
void RF_SendClearPacket(uint8_t *command)
{
	uint16_t i=0;
	
	for (i=0; i<SEND_LENGTH; i++) // clear array
		{SendBuffer[i] = 0;}
	
	SendBuffer[0] = 0xAB;
	SendBuffer[1] = 0xCD;
	SendBuffer[2] = command[2];
	SendBuffer[3] = command[3];
	SendBuffer[4] = 0xC6;
	SendBuffer[5] = 0xC6;
	SendBuffer[6] = command[6];
	SendBuffer[7] = command[7];
	SendBuffer[8] = command[8];
	SendBuffer[9] = command[9];

	for(i=0; i<SEND_PACKAGE_NUM; i++)
	{
		CC1101SendPacket(SendBuffer, SEND_LENGTH, ADDRESS_CHECK);
		HAL_Delay(5);
	}
	CC1101SetTRMode(RX_MODE, ENABLE);
	printf("Clear Command Transmit OK\n");
	RecvWaitTime = RECV_TIMEOUT;
	while((RF_Acknowledge() == 0x0 || RF_Acknowledge() == 0x01) && RecvWaitTime != 0)
	{
		RecvWaitTime--;
	}
	printf("Clear Complete\n");
}

/**
  * @brief  RF_Acknowledge(void).
  * @retval None
  */
uint8_t	RF_Acknowledge(void)
{
	uint8_t i=0, length=0;
	int16_t rssi_dBm;	
	
	if(RFReady == SET)         //?????????????? 
	{
		/* Reset transmission flag */
		RFReady = RESET;
		/* clear array */
		for (i=0; i<RECV_LENGTH; i++)   { RecvBuffer[i] = 0; } 
		length = CC1101RecPacket(RecvBuffer, &Chip_Addr, &RSSI);//???????????????
		/* Set the CC1101_IRQ_EXTI_LINE enable */
		EXTILine0_Config(GPIO_MODE_IT_FALLING, ENABLE);

		rssi_dBm = CC1101CalcRSSI_dBm(RSSI);
		printf("RSSI = %ddBm, length = %d, address = %d\n",rssi_dBm,length,Chip_Addr);
		rssi_dBm = CC1101CalcRSSI_dBm(RecvBuffer[60]);
		printf("RFID RSSI = %ddBm\n",rssi_dBm);
		for(i=0; i<RECV_LENGTH; i++)
		{
			printf("%x ",RecvBuffer[i]);
		}
		printf("\r\n");
		if(length == 0)
		{
			flag = 0x01;
			Reply_PC(flag);
			return 0x01;
		}
		else if(length == 1)
		{
			flag = 0x0C;
			Reply_PC(flag);
			return 0x01;
		}
		else
		{
				if(RecvBuffer[0] == 0xAB && RecvBuffer[1] == 0xCD)
				{
					if(RecvBuffer[4] == 0xD0 && RecvBuffer[5] == 0x01)
					{
						flag = 0x04;
						Reply_PC(flag);
						return 0x04;
					}
					else if(RecvBuffer[4] == 0xD2 && RecvBuffer[5] == 0x01)
					{
						flag = 0x05;
						Reply_PC(flag);
						return 0x05;
					}
					else if(RecvBuffer[4] == 0xD3 && RecvBuffer[5] == 0x01)
					{
						flag = 0x06;
						Reply_PC(flag);
						return 0x06;
					}
					else if(RecvBuffer[4] == 0xD4 && RecvBuffer[5] == 0x01)
					{
						flag = 0x07;
						Reply_PC(flag);
						return 0x07;
					}
					else if(RecvBuffer[4] == 0xD5 && RecvBuffer[5] == 0x01)
					{
						flag = 0x08;
						Reply_PC(flag);
						return 0x08;
					}
					else if(RecvBuffer[4] == 0xD6 && RecvBuffer[5] == 0x01)
					{
						flag = 0x09;
						Reply_PC(flag);
						return 0x09;
					}
					else if((RecvBuffer[4] == 0x01 && RecvBuffer[5] == 0x01) || (RecvBuffer[4] == 0x02 && RecvBuffer[5] == 0x02) || (RecvBuffer[4] == 0x03 && RecvBuffer[5] == 0x03))
					{
						flag = 0x0A;
						Reply_PC(flag);
						return 0x0A;
					}
					else
					{
						flag = 0x03;
						Reply_PC(flag);
						return 0x03;
					}
				}
				else
				{	
					flag = 0x02;
					Reply_PC(flag);
					return 0x02;
				}
			}
	}
	else	{	return 0x00;}
}

/**
  * @brief  Reply_PC(uint8_t flag).
  * @retval None
  */
void Reply_PC(uint8_t flag)
{
	uint8_t i=0;
		
	switch(flag)
	{
		case 0x04:
//			RTC_TimeAndDate_Access(&rtc_timestructure, &rtc_datestructure);
			AckBuffer[0] = 0xAB;
			AckBuffer[1] = 0xCD;
			AckBuffer[2] = RecvBuffer[2];
			AckBuffer[3] = RecvBuffer[3];
			AckBuffer[4] = 0xB0;
			AckBuffer[5] = 0x01;
			for(i=0;i<ACK_LLENGTH-14;i++)
			{
				AckBuffer[i+6] = RecvBuffer[i+6];
			}
			AckBuffer[61] = RSSI;
//			AckBuffer[62] = rtc_datestructure.RTC_Year;
//			AckBuffer[63] = rtc_datestructure.RTC_Month;
//			AckBuffer[64] = rtc_datestructure.RTC_Date;
//			AckBuffer[65] = rtc_datestructure.RTC_WeekDay;
//			AckBuffer[66] = rtc_timestructure.RTC_Hours;
//			AckBuffer[67] = rtc_timestructure.RTC_Minutes;
//			AckBuffer[68] = rtc_timestructure.RTC_Seconds;
			for(i=0; i<ACK_LLENGTH; i++)
			{
				printf("%x ",AckBuffer[i]);
			}
			printf("\n");
			break;
		case 0x05:
//			RTC_TimeAndDate_Access(&rtc_timestructure, &rtc_datestructure);
			AckBuffer[0] = 0xAB;
			AckBuffer[1] = 0xCD;
			AckBuffer[2] = RecvBuffer[2];
			AckBuffer[3] = RecvBuffer[3];
			AckBuffer[4] = 0xB2;
			AckBuffer[5] = 0x01;
			for(i=0;i<ACK_SLENGTH-14;i++)
			{
				AckBuffer[i+6] = RecvBuffer[i+6];
			}
			AckBuffer[19] = RSSI;
//			AckBuffer[20] = rtc_datestructure.RTC_Year;
//			AckBuffer[21] = rtc_datestructure.RTC_Month;
//			AckBuffer[22] = rtc_datestructure.RTC_Date;
//			AckBuffer[23] = rtc_datestructure.RTC_WeekDay;
//			AckBuffer[24] = rtc_timestructure.RTC_Hours;
//			AckBuffer[25] = rtc_timestructure.RTC_Minutes;
//			AckBuffer[26] = rtc_timestructure.RTC_Seconds;
			for(i=0; i<ACK_SLENGTH; i++)
			{
				printf("%x ",AckBuffer[i]);
			}
			printf("\n");
			break;
		case 0x06:
//			RTC_TimeAndDate_Access(&rtc_timestructure, &rtc_datestructure);
			AckBuffer[0] = 0xAB;
			AckBuffer[1] = 0xCD;
			AckBuffer[2] = RecvBuffer[2];
			AckBuffer[3] = RecvBuffer[3];
			AckBuffer[4] = 0xB3;
			AckBuffer[5] = 0x01;
			for(i=0;i<ACK_SLENGTH-14;i++)
			{
				AckBuffer[i+6] = RecvBuffer[i+6];
			}
			AckBuffer[19] = RSSI;
//			AckBuffer[20] = rtc_datestructure.RTC_Year;
//			AckBuffer[21] = rtc_datestructure.RTC_Month;
//			AckBuffer[22] = rtc_datestructure.RTC_Date;
//			AckBuffer[23] = rtc_datestructure.RTC_WeekDay;
//			AckBuffer[24] = rtc_timestructure.RTC_Hours;
//			AckBuffer[25] = rtc_timestructure.RTC_Minutes;
//			AckBuffer[26] = rtc_timestructure.RTC_Seconds;
			for(i=0; i<ACK_SLENGTH; i++)
			{
				printf("%x ",AckBuffer[i]);
			}
			printf("\n");
			break;
		case 0x07:
//			RTC_TimeAndDate_Access(&rtc_timestructure, &rtc_datestructure);
			AckBuffer[0] = 0xAB;
			AckBuffer[1] = 0xCD;
			AckBuffer[2] = RecvBuffer[2];
			AckBuffer[3] = RecvBuffer[3];
			AckBuffer[4] = 0xB4;
			AckBuffer[5] = 0x01;
			for(i=0;i<ACK_LLENGTH-14;i++)
			{
				AckBuffer[i+6] = RecvBuffer[i+6];
			}
			AckBuffer[61] = RSSI;
//			AckBuffer[62] = rtc_datestructure.RTC_Year;
//			AckBuffer[63] = rtc_datestructure.RTC_Month;
//			AckBuffer[64] = rtc_datestructure.RTC_Date;
//			AckBuffer[65] = rtc_datestructure.RTC_WeekDay;
//			AckBuffer[66] = rtc_timestructure.RTC_Hours;
//			AckBuffer[67] = rtc_timestructure.RTC_Minutes;
//			AckBuffer[68] = rtc_timestructure.RTC_Seconds;
			for(i=0; i<ACK_LLENGTH; i++)
			{
				printf("%x ",AckBuffer[i]);
			}
			printf("\n");
			break;
		case 0x08:
//			RTC_TimeAndDate_Access(&rtc_timestructure, &rtc_datestructure);
			AckBuffer[0] = 0xAB;
			AckBuffer[1] = 0xCD;
			AckBuffer[2] = RecvBuffer[2];
			AckBuffer[3] = RecvBuffer[3];
			AckBuffer[4] = 0xB5;
			AckBuffer[5] = 0x01;
			for(i=0;i<ACK_SLENGTH-14;i++)
			{
				AckBuffer[i+6] = RecvBuffer[i+6];
			}
			AckBuffer[19] = RSSI;
//			AckBuffer[20] = rtc_datestructure.RTC_Year;
//			AckBuffer[21] = rtc_datestructure.RTC_Month;
//			AckBuffer[22] = rtc_datestructure.RTC_Date;
//			AckBuffer[23] = rtc_datestructure.RTC_WeekDay;
//			AckBuffer[24] = rtc_timestructure.RTC_Hours;
//			AckBuffer[25] = rtc_timestructure.RTC_Minutes;
//			AckBuffer[26] = rtc_timestructure.RTC_Seconds;
			for(i=0; i<ACK_SLENGTH; i++)
			{
				printf("%x ",AckBuffer[i]);
			}
			printf("\n");
			break;
		case 0x09:
//			RTC_TimeAndDate_Access(&rtc_timestructure, &rtc_datestructure);
			AckBuffer[0] = 0xAB;
			AckBuffer[1] = 0xCD;
			AckBuffer[2] = RecvBuffer[2];
			AckBuffer[3] = RecvBuffer[3];
			AckBuffer[4] = 0xB6;
			AckBuffer[5] = 0x01;
			for(i=0;i<ACK_SLENGTH-14;i++)
			{
				AckBuffer[i+6] = RecvBuffer[i+6];
			}
			AckBuffer[19] = RSSI;
//			AckBuffer[20] = rtc_datestructure.RTC_Year;
//			AckBuffer[21] = rtc_datestructure.RTC_Month;
//			AckBuffer[22] = rtc_datestructure.RTC_Date;
//			AckBuffer[23] = rtc_datestructure.RTC_WeekDay;
//			AckBuffer[24] = rtc_timestructure.RTC_Hours;
//			AckBuffer[25] = rtc_timestructure.RTC_Minutes;
//			AckBuffer[26] = rtc_timestructure.RTC_Seconds;
			for(i=0; i<ACK_SLENGTH; i++)
			{
				printf("%x ",AckBuffer[i]);
			}
			printf("\n");
			break;
		case 0x0A:
//			RTC_TimeAndDate_Access(&rtc_timestructure, &rtc_datestructure);
			AckBuffer[0] = 0xAB;
			AckBuffer[1] = 0xCD;
			AckBuffer[2] = RecvBuffer[2];
			AckBuffer[3] = RecvBuffer[3];
			AckBuffer[4] = 0xB7;
			AckBuffer[5] = 0x01;
			for(i=0;i<ACK_SLENGTH-14;i++)
			{
				AckBuffer[i+6] = RecvBuffer[i+6];
			}
			AckBuffer[19] = RSSI;
//			AckBuffer[20] = rtc_datestructure.RTC_Year;
//			AckBuffer[21] = rtc_datestructure.RTC_Month;
//			AckBuffer[22] = rtc_datestructure.RTC_Date;
//			AckBuffer[23] = rtc_datestructure.RTC_WeekDay;
//			AckBuffer[24] = rtc_timestructure.RTC_Hours;
//			AckBuffer[25] = rtc_timestructure.RTC_Minutes;
//			AckBuffer[26] = rtc_timestructure.RTC_Seconds;
			for(i=0; i<ACK_SLENGTH; i++)
			{
				printf("%x ",AckBuffer[i]);
			}
			printf("\n");
			break;
//		case 0x0B:
//			RTC_TimeAndDate_Access(&rtc_timestructure, &rtc_datestructure);
//			AckBuffer[0] = 0xAB;
//			AckBuffer[1] = 0xCD;
//			AckBuffer[2] = PCCommand[2];
//			AckBuffer[3] = PCCommand[3];
//			AckBuffer[4] = 0xB1;
//			AckBuffer[5] = PCCommand[5];
//			AckBuffer[6] = PCCommand[6];
//			AckBuffer[7] = PCCommand[7];
//			AckBuffer[8] = PCCommand[8];
//			AckBuffer[9] = PCCommand[9];
//			for(i=0;i<10;i++)
//			{
//				AckBuffer[i+10] = 0;
//			}
//			AckBuffer[20] = rtc_datestructure.RTC_Year;
//			AckBuffer[21] = rtc_datestructure.RTC_Month;
//			AckBuffer[22] = rtc_datestructure.RTC_Date;
//			AckBuffer[23] = rtc_datestructure.RTC_WeekDay;
//			AckBuffer[24] = rtc_timestructure.RTC_Hours;
//			AckBuffer[25] = rtc_timestructure.RTC_Minutes;
//			AckBuffer[26] = rtc_timestructure.RTC_Seconds;
//				for(i=0; i<ACK_SLENGTH; i++)
//				{
//					printf("%x ",AckBuffer[i]);
//				}
//				printf("\n");
//			printf("The Date :  Y:20%0.2d - M:%0.2d - D:%0.2d - W:%0.2d\r\n", rtc_datestructure.RTC_Year,rtc_datestructure.RTC_Month, rtc_datestructure.RTC_Date,rtc_datestructure.RTC_WeekDay);
//			printf("The Time :  %0.2d:%0.2d:%0.2d \r\n\r\n", rtc_timestructure.RTC_Hours, rtc_timestructure.RTC_Minutes, rtc_timestructure.RTC_Seconds);
//			break;
		case 0x01:
			printf("receive error or Address Filtering fail\n");
			break;
		case 0x02:
			printf("RFID receive package beginning error\r\n");
			break;
		case 0x03:
			printf("RFID receive function order error\r\n");
			break;
		case 0x0C:
			printf("RFID receive crc error\r\n");
			break;
		default : break;
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
