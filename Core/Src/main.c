/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "crc.h"
#include "dma.h"
#include "i2c.h"
#include "iwdg.h"
#include "lptim.h"
#include "usart.h"
#include "rtc.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cc1101.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
__IO uint32_t CollectorID;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
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
  // MX_DMA_Init(); must before MX_USART1_UART_Init(); or MX_LPUART1_UART_Init();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  MX_IWDG_Init();
  MX_RTC_Init();
  MX_CRC_Init();
  MX_LPUART1_UART_Init();
  MX_I2C1_Init();
  MX_LPTIM1_Init();
  /* USER CODE BEGIN 2 */
  SystemInitial();
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

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
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_8;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_3;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_LPUART1
                              |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_RTC
                              |RCC_PERIPHCLK_LPTIM1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.LptimClockSelection = RCC_LPTIM1CLKSOURCE_LSE;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
// ##################################################################################################################
void SystemInitial(void)
{
  Activate_SPI();

  if (HAL_IWDG_Refresh(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }

  ModuleLtePowerOn();
  CollectorID = DATAEEPROM_Read(EEPROM_START_ADDR);
  CC1101_POWER_ON();
  RFIDInitial(0xEF, 0x1234, RX_MODE);
  HAL_Delay(10000);

  if (HAL_IWDG_Refresh(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_Delay(10000);

  if (HAL_IWDG_Refresh(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
}
// ##################################################################################################################
void ShowMessage(void)
{
  /* Print Open Massage */
  printf("######################################################################\n");
  printf("#------------------Collector Program Start Running-------------------#\n");
  printf("#------------------------CollectorID:%08x------------------------#\n", CollectorID);
  printf("#--------------Using LPUART1,Configuration:%d 8-N-1--------------#\n", 115200);
  printf("#USART RX Method: DMA HT & TC + USART IDLE LINE IRQ + RTOS processing#\n");
  printf("######################################################################\n");
}
// ##################################################################################################################
void ModuleLtePowerOn(void)
{
  USR4G_POWER_KEY_OFF();
  HAL_Delay(2000);
  USR4G_POWER_KEY_ON();
  printf("##LTE Module Power On Complete##\n");
}
// ##################################################################################################################
void strcatArray(uint8_t *dest, uint8_t *src, uint8_t position, uint8_t srclen)
{
  if (srclen == 0x00)
  {
    for (uint8_t i = 0; i < sizeof(src); i++)
      dest[i + position] = src[i];
  }
  else
  {
    for (uint8_t i = 0; i < srclen; i++)
      dest[i + position] = src[i];
  }
}
// ##################################################################################################################
uint8_t AsciiToHex(uint8_t *pAscii, uint8_t *pHex, uint16_t nLen)
{
  uint16_t nHexLen = nLen / 2;
  uint8_t Nibble[2] = {0};
  uint16_t i = 0;
  uint16_t j = 0;

  if (nLen % 2)
  {
    return 1;
  }

  for (i = 0; i < nHexLen; i++)
  {
    Nibble[0] = *pAscii++;
    Nibble[1] = *pAscii++;

    for (j = 0; j < 2; j++)
    {
      if (Nibble[j] <= 'F' && Nibble[j] >= 'A')
        Nibble[j] = Nibble[j] - 'A' + 10;
      else if (Nibble[j] <= 'f' && Nibble[j] >= 'a')
        Nibble[j] = Nibble[j] - 'a' + 10;
      else if (Nibble[j] >= '0' && Nibble[j] <= '9')
        Nibble[j] = Nibble[j] - '0';
      else
        return 1; // Nibble[j] = Nibble[j] - 'a' + 10;

    } // for (int j = ...)

    pHex[i] = Nibble[0] << 4; // Set the high nibble
    pHex[i] |= Nibble[1];     // Set the low nibble
  } // for (int i = ...)

  return 0;
}
// ##################################################################################################################
void DATAEEPROM_Program(uint32_t Address, uint32_t Data)
{
  /* Unlocks the data memory and FLASH_PECR register access *************/
  if (HAL_FLASHEx_DATAEEPROM_Unlock() != HAL_OK)
  {
    Error_Handler();
  }

  /* Clear FLASH error pending bits */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_WRPERR | FLASH_FLAG_SIZERR |
                         FLASH_FLAG_OPTVERR | FLASH_FLAG_RDERR |
                         FLASH_FLAG_FWWERR | FLASH_FLAG_NOTZEROERR);

  /*Erase a word in data memory *************/
  if (HAL_FLASHEx_DATAEEPROM_Erase(Address) != HAL_OK)
  {
    Error_Handler();
  }

  /*Enable DATA EEPROM fixed Time programming (2*Tprog) *************/
  HAL_FLASHEx_DATAEEPROM_EnableFixedTimeProgram();

  /* Program word at a specified address *************/
  if (HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, Address, Data) != HAL_OK)
  {
    Error_Handler();
  }

  /*Disables DATA EEPROM fixed Time programming (2*Tprog) *************/
  HAL_FLASHEx_DATAEEPROM_DisableFixedTimeProgram();

  /* Locks the Data memory and FLASH_PECR register access. (recommended
   to protect the DATA_EEPROM against possible unwanted operation) *********/
  HAL_FLASHEx_DATAEEPROM_Lock();
}
// ##################################################################################################################
uint32_t DATAEEPROM_Read(uint32_t Address)
{
  return *(__IO uint32_t *)Address;
}
// ##################################################################################################################
void LED_Blinking(uint32_t Period)
{
  /* Toggle LED in an infinite loop */
  while (1)
  {
    LED_GREEN_TOG();
    HAL_Delay(Period);
  }
}
// ##################################################################################################################
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  _Error_Handler(__FILE__, __LINE__);
  LED_Blinking(200);
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();

  while (1)
  {
  }

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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
