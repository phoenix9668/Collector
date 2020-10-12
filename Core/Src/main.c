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
#include "iwdg.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cc1101.h"
#include "fram.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern TIM_HandleTypeDef htim6;
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
extern __IO ITStatus rxCatch;
extern __IO ITStatus txFiFoUnFlow;
extern __IO uint32_t basetime;

extern __IO uint8_t MBID_byte1;
extern __IO uint8_t MBID_byte2;
extern __IO uint16_t INTERVAL;
extern __IO uint8_t RFID_init[RFID_SUM][FRAM_DATA_LENGTH]; 

uint8_t sendBuffer[SEND_A7_LENGTH] = {0};
uint8_t recvBuffer[RECV_LENGTH] = {0};
uint8_t ackBuffer[ACK_LLENGTH] = {0};
uint8_t length;
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
  MX_TIM6_Init();
  MX_RTC_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
	MOD_GPRS_RESET();
	Activate_SPI();
	Activate_USART3_RXIT();
	HAL_TIM_Base_Start_IT(&htim6);
	RFIDInitial(0x20, 0x2020, RX_MODE);
	Init_RFID_Info();
	HAL_Delay(8000);
	Show_Message();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		LL_IWDG_ReloadCounter(IWDG);
		if(INTERVAL != 0)
		{
			if(basetime == INTERVAL)
			{
				LED_COM_ON();
				basetime = 0;
				Polling_All_RFID();
				LED_COM_OFF();
			}
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		if(CommandState == SET)
		{
			LED_STA_ON();
			if(RxBuffer[0] == 0xE5 && RxBuffer[1] == 0x5E)
			{
				printf("fram transfer\n");
				FRAM_Ctrl(RxBuffer);
			}
			else if(RxBuffer[0] == MBID_byte1 && RxBuffer[1] == MBID_byte2)
			{
				printf("start transfer\n");
				Function_Ctrl(RxBuffer);
			}
			else
			{
				printf("device number or command error\n");
			}
			CommandState = RESET;
			LED_STA_OFF();
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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
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
	printf("Time Interval:%x\n",INTERVAL);
}


/*===========================================================================
* Polling_All_RFID(void) => polling all rfid                    						* 
============================================================================*/
void Polling_All_RFID(void)
{
	uint16_t syncword;
	uint32_t device_code1,device_code2,device_code3;
	uint8_t tempArray[SEND_A026_LENGTH];
	uint16_t i;
	uint8_t j;
	uint8_t triple,err;
	
	for(i=0; i<RFID_SUM; i++)
	{
		device_code1 = (uint32_t)(0xFF000000 & RFID_init[i][0]<<24)+(uint32_t)(0x00FF0000 & RFID_init[i][1]<<16)+(uint32_t)(0x0000FF00 & RFID_init[i][2]<<8)+(uint32_t)(0x000000FF & RFID_init[i][3]);
		device_code2 = (uint32_t)(0xFF000000 & RFID_init[i][4]<<24)+(uint32_t)(0x00FF0000 & RFID_init[i][5]<<16)+(uint32_t)(0x0000FF00 & RFID_init[i][6]<<8)+(uint32_t)(0x000000FF & RFID_init[i][7]);
		device_code3 = (uint32_t)(0xFF000000 & RFID_init[i][8]<<24)+(uint32_t)(0x00FF0000 & RFID_init[i][9]<<16)+(uint32_t)(0x0000FF00 & RFID_init[i][10]<<8)+(uint32_t)(0x000000FF & RFID_init[i][11]);
		if(device_code1 != 0 || device_code2 != 0 || device_code3 != 0)
		{
			syncword = (uint16_t)(0xFF00 & RFID_init[i][14]<<8)+(uint16_t)(0x00FF & RFID_init[i][15]);
			tempArray[0] = MBID_byte1;
			tempArray[1] = MBID_byte2;
			tempArray[2] = 0xA0;
			for(j=0; j<12; j++)
			{	tempArray[j+3] = RFID_init[i][j];}
			RFIDInitial(RFID_init[i][13], syncword, RX_MODE);
			triple = 0x03;
			err = 0x01;
			while((err != 0x00) && (triple != 0x00))
			{
				err = RF_SendPacket(tempArray,SEND_A026_LENGTH);
				triple--;
				HAL_Delay(100);
			}
			LL_IWDG_ReloadCounter(IWDG);
			printf("scaning:%d\n",i);
		}
	}
	printf("All Finish\n");
}

/*===========================================================================
* Function_Ctrl(uint8_t *command) => handle PC command                    	* 
============================================================================*/
void Function_Ctrl(uint8_t *command)
{
	uint8_t i;
	uint8_t timeBuffer[3];
	uint8_t dateBuffer[4];
	
	/*A0:check assign rfid*/
	/*A2:clear assign RFID battery flag*/
	/*A3:configure assign RFID's ADXL362 parameter*/
	/*A5:configure RFID addr sync unicode*/
	/*A6:clear assign RFID data*/
	/*A8:configure time information*/
	/*A9:read time information*/
	if(command[2] == 0xA0 || command[2] == 0xA2 || command[2] == 0xA3 || command[2] == 0xA5 || command[2] == 0xA6 || command[2] == 0xA7)
	{
		Check_Assign_RFID(command);
	}
	else if(command[2] == 0xA8)
	{
		for(i=0; i<3; i++)
		{	timeBuffer[i] = command[7+i];}
		for(i=0; i<4; i++)
		{	dateBuffer[i] = command[3+i];}
			
		SetRTC(timeBuffer, dateBuffer);
		Reply_PC(command[2], ACK_SLENGTH);
	}
	else if(command[2] == 0xA9)
	{
		Reply_PC(command[2], ACK_SLENGTH);
	}
	else
		printf("function order error\n");
}

/*===========================================================================
* Check_Assign_RFID() => check assign rfid                    							* 
============================================================================*/
void Check_Assign_RFID(uint8_t *command)
{
	uint32_t rfid1,rfid2,rfid3,device_code1,device_code2,device_code3;
	uint16_t syncword;
	uint16_t i=0;
	uint8_t err=0;
	
	rfid1 = (uint32_t)(0xFF000000 & command[3]<<24)+(uint32_t)(0x00FF0000 & command[4]<<16)+(uint32_t)(0x0000FF00 & command[5]<<8)+(uint32_t)(0x000000FF & command[6]);
	rfid2 = (uint32_t)(0xFF000000 & command[7]<<24)+(uint32_t)(0x00FF0000 & command[8]<<16)+(uint32_t)(0x0000FF00 & command[9]<<8)+(uint32_t)(0x000000FF & command[10]);
	rfid3 = (uint32_t)(0xFF000000 & command[11]<<24)+(uint32_t)(0x00FF0000 & command[12]<<16)+(uint32_t)(0x0000FF00 & command[13]<<8)+(uint32_t)(0x000000FF & command[14]);
	
	for(i=0; i<RFID_SUM; i++)
	{
		device_code1 = (uint32_t)(0xFF000000 & RFID_init[i][0]<<24)+(uint32_t)(0x00FF0000 & RFID_init[i][1]<<16)+(uint32_t)(0x0000FF00 & RFID_init[i][2]<<8)+(uint32_t)(0x000000FF & RFID_init[i][3]);
		device_code2 = (uint32_t)(0xFF000000 & RFID_init[i][4]<<24)+(uint32_t)(0x00FF0000 & RFID_init[i][5]<<16)+(uint32_t)(0x0000FF00 & RFID_init[i][6]<<8)+(uint32_t)(0x000000FF & RFID_init[i][7]);
		device_code3 = (uint32_t)(0xFF000000 & RFID_init[i][8]<<24)+(uint32_t)(0x00FF0000 & RFID_init[i][9]<<16)+(uint32_t)(0x0000FF00 & RFID_init[i][10]<<8)+(uint32_t)(0x000000FF & RFID_init[i][11]);
		if(rfid1 == device_code1 && rfid2 == device_code2 && rfid3 == device_code3)
		{
			syncword = (uint16_t)(0xFF00 & RFID_init[i][14]<<8)+(uint16_t)(0x00FF & RFID_init[i][15]);
			RFIDInitial(RFID_init[i][13], syncword, RX_MODE);
			if(command[2] == 0xA0 || command[2] == 0xA2 || command[2] == 0xA6)
				{RF_SendPacket(command, SEND_A026_LENGTH);}
			else if(command[2] == 0xA3)
				{RF_SendPacket(command, SEND_A3_LENGTH);}
			else if(command[2] == 0xA5)
				{RF_SendPacket(command, SEND_A5_LENGTH);}
			else if(command[2] == 0xA7)
				{RF_SendPacket(command, SEND_A7_LENGTH);}
			err = 1;
		}
	}
	if(err == 0)
	{
		printf("RFID coding error\n");
	}
}

/*===========================================================================
* RF_SendPacket(uint8_t *command, uint8_t length) => send assign rfid data 	* 
============================================================================*/
uint8_t RF_SendPacket(uint8_t *buffer, uint8_t size)
{
	uint16_t i=0;
	uint32_t tickstart = 0U;
	uint8_t ack;
	uint8_t err;
	
	err = 0x01;
	if(buffer[2] == 0xA0)
		{buffer[2] = 0xC0;}
	else if(buffer[2] == 0xA2)
		{buffer[2] = 0xC2;}
	else if(buffer[2] == 0xA3)
		{buffer[2] = 0xC3;}
	else if(buffer[2] == 0xA5)
		{buffer[2] = 0xC5;}
	else if(buffer[2] == 0xA6)
		{buffer[2] = 0xC6;}
	else if(buffer[2] == 0xA7)
		{buffer[2] = 0xC7;}

//	tickstart = HAL_GetTick();
//	printf("tickstart is %d\n",tickstart);
		
	for(i=0; i<SEND_PACKAGE_NUM; i++)
	{
		CC1101SendPacket(buffer, size, ADDRESS_CHECK);
		HAL_Delay(1);
	}
//	tickstart = HAL_GetTick();
//	printf("tickend is %d\n",tickstart);
	
	CC1101SetTRMode(RX_MODE);
	printf("Transmit OK\n");
	/* Init tickstart for timeout managment */
	tickstart = HAL_GetTick();
	
	while((HAL_GetTick() - tickstart) < RecvTimeout)
	{
		ack = RF_Acknowledge(buffer);
		Reply_PC(ack, length);
		if(ack == 0xD0 || ack == 0xD2 || ack == 0xD3 || ack == 0xD5 || ack == 0xD6 || ack == 0xD7)
		{
			err = 0x00;
		}
	}
	
	if(buffer[2] == 0xC0)
		{	printf("Transmit Complete\n");}
	else if(buffer[2] == 0xC6)
		{	printf("Clear Complete\n");}
	else if(buffer[2] == 0xC2 || buffer[2] == 0xC3 || buffer[2] == 0xC5 || buffer[2] == 0xC7)
		{	printf("Program Complete\n");}
	else
		{	printf("ack is %x\n",buffer[2]);}
	
	return err;
}


/*===========================================================================
* RF_Acknowledge(uint8_t *buffer) => waiting receive assign rfid data 			* 
============================================================================*/
uint8_t	RF_Acknowledge(uint8_t *buffer)
{
	uint8_t i=0;
	int16_t rssi_dBm;
	
	if(rxCatch == SET)
	{
		/*##-1- Reset transmission flag ###################################*/
		rxCatch = RESET;
		txFiFoUnFlow = RESET;
		/*##-2- clear array ###################################*/
		for (i=0; i<RECV_LENGTH; i++)
		{ recvBuffer[i] = 0;} 
		/*##-3- Wait for the trigger of the threshold ###################################*/ 
		while (txFiFoUnFlow != SET && rxCatch != SET){}
		length = CC1101RecPacket(recvBuffer, &Chip_Addr, &RSSI);

		rssi_dBm = CC1101CalcRSSI_dBm(RSSI);
		printf("RSSI = %ddBm, length = %d, address = %d\n",rssi_dBm,length,Chip_Addr);
		rssi_dBm = CC1101CalcRSSI_dBm(recvBuffer[length-1]);
		printf("RFID RSSI = %ddBm\n",rssi_dBm);
		for(i=0; i<length; i++)
		{	printf("%x ",recvBuffer[i]);}
		printf("\r\n");
		
		/* Reset transmission flag */
		rxCatch = RESET;
		
		if(length == 0)
		{	return 0x01;}
		else if(length == 1)
		{	return 0x02;}
		else
		{
			if(recvBuffer[3] == buffer[3] && recvBuffer[4] == buffer[4] && recvBuffer[5] == buffer[5] && recvBuffer[6] == buffer[6] && recvBuffer[7] == buffer[7] && recvBuffer[8] == buffer[8]
					&& recvBuffer[9] == buffer[9] && recvBuffer[10] == buffer[10] && recvBuffer[11] == buffer[11] && recvBuffer[12] == buffer[12] && recvBuffer[13] == buffer[13] && recvBuffer[14] == buffer[14])
				{
					if(recvBuffer[2] == 0xD0 || recvBuffer[2] == 0xD2 || recvBuffer[2] == 0xD3 || recvBuffer[2] == 0xD5 || recvBuffer[2] == 0xD6 || recvBuffer[2] == 0xD7)
					{	return recvBuffer[2];}
					else if(recvBuffer[2] == 0xE1 || recvBuffer[2] == 0xE2 || recvBuffer[2] == 0xE3)
					{	return recvBuffer[2];}
					else
					{	return 0x03;}
				}
				else
				{	return 0x04;}
		}
	}
	else	{	return 0x00;}
}

/**
  * @brief  Reply_PC(uint8_t ack, uint8_t length).
  * @retval None
  */
void Reply_PC(uint8_t ack, uint8_t length)
{
	uint8_t i=0;
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
		
	if(ack == 0xD0 || ack == 0xD2 || ack == 0xD3 || ack == 0xD5 || ack == 0xD6 || ack == 0xD7)
	{
		GetRTC(&sTime, &sDate);
		for(i=0;i<length;i++)
		{
			ackBuffer[i] = recvBuffer[i];
		}
		ackBuffer[2] = 0xB0 + (0x0F & recvBuffer[2]);
		ackBuffer[length] = RSSI;
		ackBuffer[length+1] = sDate.Year;
		ackBuffer[length+2] = sDate.Month;
		ackBuffer[length+3] = sDate.Date;
		ackBuffer[length+4] = sDate.WeekDay;
		ackBuffer[length+5] = sTime.Hours;
		ackBuffer[length+6] = sTime.Minutes;
		ackBuffer[length+7] = sTime.Seconds;
		for(i=0; i<length+8; i++)
		{
			printf("%x ",ackBuffer[i]);
		}
		printf("\n");
	}
	else if(ack == 0xA8 || ack == 0xA9)
	{
		GetRTC(&sTime, &sDate);
		for(i=0;i<2;i++)
		{
			ackBuffer[i] = recvBuffer[i];
		}
		ackBuffer[2] = 0xB0 + (0x0F & recvBuffer[2]);
		ackBuffer[3] = sDate.Year;
		ackBuffer[4] = sDate.Month;
		ackBuffer[5] = sDate.Date;
		ackBuffer[6] = sDate.WeekDay;
		ackBuffer[7] = sTime.Hours;
		ackBuffer[8] = sTime.Minutes;
		ackBuffer[9] = sTime.Seconds;
		for(i=0; i<length; i++)
		{
			printf("%x ",ackBuffer[i]);
		}
		printf("\n");
	}
	else if(ack == 0xE1 || ack == 0xE2 || ack == 0xE3)
	{
		GetRTC(&sTime, &sDate);
		for(i=0;i<length;i++)
		{
			ackBuffer[i] = recvBuffer[i];
		}
		ackBuffer[length] = RSSI;
		ackBuffer[length+1] = sDate.Year;
		ackBuffer[length+2] = sDate.Month;
		ackBuffer[length+3] = sDate.Date;
		ackBuffer[length+4] = sDate.WeekDay;
		ackBuffer[length+5] = sTime.Hours;
		ackBuffer[length+6] = sTime.Minutes;
		ackBuffer[length+7] = sTime.Seconds;
		for(i=0; i<length+8; i++)
		{
			printf("%x ",ackBuffer[i]);
		}
		printf("\n");
	}
	else if(ack == 0x01)
	{	printf("receive error or Address Filtering fail\n");}
	else if(ack == 0x02)
	{	printf("RFID receive crc error\r\n");}
	else if(ack == 0x03)
	{	printf("RFID receive function order error\r\n");}
	else if(ack == 0x04)
	{	printf("RFID receive RFID code error\r\n");}

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
