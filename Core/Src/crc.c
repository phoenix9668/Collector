/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    crc.c
  * @brief   This file provides code for the configuration
  *          of the CRC instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "crc.h"

/* USER CODE BEGIN 0 */
/** @defgroup CRC_Private_Functions CRC Private Functions
  * @{
  */
static uint32_t CRC_Handle_8(CRC_HandleTypeDef *hcrc, uint8_t pBuffer[], uint32_t BufferLength);
static uint32_t CRC_Handle_16(CRC_HandleTypeDef *hcrc, uint16_t pBuffer[], uint32_t BufferLength);
/* USER CODE END 0 */

CRC_HandleTypeDef hcrc;

/* CRC init function */
void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

void HAL_CRC_MspInit(CRC_HandleTypeDef* crcHandle)
{

  if(crcHandle->Instance==CRC)
  {
  /* USER CODE BEGIN CRC_MspInit 0 */

  /* USER CODE END CRC_MspInit 0 */
    /* CRC clock enable */
    __HAL_RCC_CRC_CLK_ENABLE();
  /* USER CODE BEGIN CRC_MspInit 1 */

  /* USER CODE END CRC_MspInit 1 */
  }
}

void HAL_CRC_MspDeInit(CRC_HandleTypeDef* crcHandle)
{

  if(crcHandle->Instance==CRC)
  {
  /* USER CODE BEGIN CRC_MspDeInit 0 */

  /* USER CODE END CRC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CRC_CLK_DISABLE();
  /* USER CODE BEGIN CRC_MspDeInit 1 */

  /* USER CODE END CRC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
uint32_t CRC32_Check(uint8_t pBuffer[], uint32_t BufferLength, InputDataFormat inputDataFormat)
{
	uint32_t crc32Calc = 0U,crc32Value = 0U;  /* CRC output (read from hcrc->Instance->DR register) */
	uint8_t uLen;
	uint8_t tbuffer[BufferLength];
	
	memset(tbuffer, 0, sizeof(tbuffer));
	if ((BufferLength % 4U) != 0U)
	{ uLen = 4U - BufferLength & 0x03;}
	else
	{ uLen = 0;}
	debug_printf("uLen = %x\n",uLen);
	
	crc32Value = ((uint32_t)pBuffer[BufferLength - 4U] << 24U) | \
							 ((uint32_t)pBuffer[BufferLength - 3U] << 16U) | \
							 ((uint32_t)pBuffer[BufferLength - 2U] << 8U)  | \
							 (uint32_t)pBuffer[BufferLength - 1U];
	debug_printf("crc32Value = %x\n",crc32Value);
	
	for (uint8_t i=0; i<BufferLength - 4; i++)
	{	tbuffer[i] = pBuffer[i];}
	
	for (uint32_t i=0; i<BufferLength; i++)
	{ tbuffer[i] = (uint8_t)(__RBIT(tbuffer[i])>>24);}
	
	crc32Calc = HAL_CRC_Calculate_U(&hcrc, (uint32_t *)tbuffer, BufferLength - 4 + uLen, inputDataFormat);
	crc32Calc = __RBIT(crc32Calc);
	crc32Calc = crc32Calc^0xFFFFFFFF;
	debug_printf("BufferLength = %d\n", BufferLength - 4 + uLen);
	debug_printf("crc32Calc = %x\n",crc32Calc);
	
	/* Return the CRC check */
	if ( crc32Calc == crc32Value)
    return 0x00;
	else
		return 0x01;
}

uint32_t CRC32_Calculate(uint8_t pBuffer[], uint32_t BufferLength, InputDataFormat inputDataFormat)
{
	uint32_t crc32Calc = 0U;  /* CRC output (read from hcrc->Instance->DR register) */
	uint8_t uLen;
	uint8_t tbuffer[BufferLength];
	
	memset(tbuffer, 0, sizeof(tbuffer));
	if ((BufferLength % 4U) != 0U)
	{ uLen = 4U - BufferLength & 0x03;}
	else
	{ uLen = 0;}
	debug_printf("uLen = %x\n",uLen);
	
	for (uint8_t i=0; i<BufferLength; i++)
	{	tbuffer[i] = pBuffer[i];}
	
	for (uint32_t i=0; i<BufferLength; i++)
	{ tbuffer[i] = (uint8_t)(__RBIT(tbuffer[i])>>24);}
	
	crc32Calc = HAL_CRC_Calculate_U(&hcrc, (uint32_t *)tbuffer, BufferLength + uLen, inputDataFormat);
	crc32Calc = __RBIT(crc32Calc);
	crc32Calc = crc32Calc^0xFFFFFFFF;
	debug_printf("BufferLength = %d\n", BufferLength + uLen);
	debug_printf("crc32Calc = %x\n",crc32Calc);
	
  /* Return the CRC computed value */
  return crc32Calc;
}
/**
  * @brief  Compute the 7, 8, 16 or 32-bit CRC value of an 8, 16 or 32-bit data buffer
  *         starting with hcrc->Instance->INIT as initialization value.
  * @param  hcrc CRC handle
  * @param  pBuffer pointer to the input data buffer, exact input data format is
  *         provided by hcrc->InputDataFormat.
  * @param  BufferLength input data buffer length (number of bytes if pBuffer
  *         type is * uint8_t, number of half-words if pBuffer type is * uint16_t,
  *         number of words if pBuffer type is * uint32_t).
  * @note  By default, the API expects a uint32_t pointer as input buffer parameter.
  *        Input buffer pointers with other types simply need to be cast in uint32_t
  *        and the API will internally adjust its input data processing based on the
  *        handle field hcrc->InputDataFormat.
  * @retval uint32_t CRC (returned value LSBs for CRC shorter than 32 bits)
  */
uint32_t HAL_CRC_Calculate_U(CRC_HandleTypeDef *hcrc, uint32_t pBuffer[], uint32_t BufferLength, InputDataFormat inputDataFormat)
{
  uint32_t index;      /* CRC input data buffer index */
  uint32_t temp = 0U;  /* CRC output (read from hcrc->Instance->DR register) */

  /* Change CRC peripheral state */
  hcrc->State = HAL_CRC_STATE_BUSY;

  /* Reset CRC Calculation Unit (hcrc->Instance->INIT is
  *  written in hcrc->Instance->DR) */
  __HAL_CRC_DR_RESET(hcrc);

  switch (inputDataFormat)
  {
    case CRC_INPUTDATA_FORMAT_WORDS:
      /* Enter 32-bit input data to the CRC calculator */
      for (index = 0U; index < BufferLength; index++)
      {
        hcrc->Instance->DR = pBuffer[index];
      }
      temp = hcrc->Instance->DR;
      break;

    case CRC_INPUTDATA_FORMAT_BYTES:
      /* Specific 8-bit input data handling  */
      temp = CRC_Handle_8(hcrc, (uint8_t *)pBuffer, BufferLength);
      break;

    case CRC_INPUTDATA_FORMAT_HALFWORDS:
      /* Specific 16-bit input data handling  */
      temp = CRC_Handle_16(hcrc, (uint16_t *)(void *)pBuffer, BufferLength);    /* Derogation MisraC2012 R.11.5 */
      break;

    default:
      break;
  }

  /* Change CRC peripheral state */
  hcrc->State = HAL_CRC_STATE_READY;

  /* Return the CRC computed value */
  return temp;
}



/**
  * @brief  Enter 8-bit input data to the CRC calculator.
  *         Specific data handling to optimize processing time.
  * @param  hcrc CRC handle
  * @param  pBuffer pointer to the input data buffer
  * @param  BufferLength input data buffer length
  * @retval uint32_t CRC (returned value LSBs for CRC shorter than 32 bits)
  */
static uint32_t CRC_Handle_8(CRC_HandleTypeDef *hcrc, uint8_t pBuffer[], uint32_t BufferLength)
{
  uint32_t i; /* input data buffer index */
  uint16_t data;
  __IO uint16_t *pReg;

  /* Processing time optimization: 4 bytes are entered in a row with a single word write,
   * last bytes must be carefully fed to the CRC calculator to ensure a correct type
   * handling by the peripheral */
  for (i = 0U; i < (BufferLength / 4U); i++)
  {
    hcrc->Instance->DR = ((uint32_t)pBuffer[4U * i] << 24U) | \
                         ((uint32_t)pBuffer[(4U * i) + 1U] << 16U) | \
                         ((uint32_t)pBuffer[(4U * i) + 2U] << 8U)  | \
                         (uint32_t)pBuffer[(4U * i) + 3U];
  }
  /* last bytes specific handling */
  if ((BufferLength % 4U) != 0U)
  {
    if ((BufferLength % 4U) == 1U)
    {
      *(__IO uint8_t *)(__IO void *)(&hcrc->Instance->DR) = pBuffer[4U * i];         /* Derogation MisraC2012 R.11.5 */
    }
    if ((BufferLength % 4U) == 2U)
    {
      data = ((uint16_t)(pBuffer[4U * i]) << 8U) | (uint16_t)pBuffer[(4U * i) + 1U];
      pReg = (__IO uint16_t *)(__IO void *)(&hcrc->Instance->DR);                    /* Derogation MisraC2012 R.11.5 */
      *pReg = data;
    }
    if ((BufferLength % 4U) == 3U)
    {
      data = ((uint16_t)(pBuffer[4U * i]) << 8U) | (uint16_t)pBuffer[(4U * i) + 1U];
      pReg = (__IO uint16_t *)(__IO void *)(&hcrc->Instance->DR);                    /* Derogation MisraC2012 R.11.5 */
      *pReg = data;

      *(__IO uint8_t *)(__IO void *)(&hcrc->Instance->DR) = pBuffer[(4U * i) + 2U];  /* Derogation MisraC2012 R.11.5 */
    }
  }

  /* Return the CRC computed value */
  return hcrc->Instance->DR;
}

/**
  * @brief  Enter 16-bit input data to the CRC calculator.
  *         Specific data handling to optimize processing time.
  * @param  hcrc CRC handle
  * @param  pBuffer pointer to the input data buffer
  * @param  BufferLength input data buffer length
  * @retval uint32_t CRC (returned value LSBs for CRC shorter than 32 bits)
  */
static uint32_t CRC_Handle_16(CRC_HandleTypeDef *hcrc, uint16_t pBuffer[], uint32_t BufferLength)
{
  uint32_t i;  /* input data buffer index */
  __IO uint16_t *pReg;

  /* Processing time optimization: 2 HalfWords are entered in a row with a single word write,
   * in case of odd length, last HalfWord must be carefully fed to the CRC calculator to ensure
   * a correct type handling by the peripheral */
  for (i = 0U; i < (BufferLength / 2U); i++)
  {
    hcrc->Instance->DR = ((uint32_t)pBuffer[2U * i] << 16U) | (uint32_t)pBuffer[(2U * i) + 1U];
  }
  if ((BufferLength % 2U) != 0U)
  {
    pReg = (__IO uint16_t *)(__IO void *)(&hcrc->Instance->DR);                 /* Derogation MisraC2012 R.11.5 */
    *pReg = pBuffer[2U * i];
  }

  /* Return the CRC computed value */
  return hcrc->Instance->DR;
}
/* USER CODE END 1 */
