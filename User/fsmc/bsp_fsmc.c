/**
  ******************************************************************************
  * @file    bsp_fsmc.c
  * @author  phoenix
  * @version V1.0
  * @date    16-January-2018
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */ 
  
#include "./fsmc/bsp_fsmc.h" 

/**
  * @brief  FSMC GPIO CONFIG
	*							DB0--PD14  |  DB1--PD15  |  DB2--PD0  |  DB3--PD1  
  *							DB4--PE7   |  DB5--PE8   |  DB6--PE9  |  DB7--PE10  
  *							RD---PD4   |  WR---PD5   |
	*							NAND PORT:  PD3--FSMC_CLK | PD6--FSMC_NWAIT | PD7--FSMC_NCE2 | PD11--FSMC_A16(CLE) | PD12--FSMC_A17(ALE)  
  * @param  none
  * @retval	none
  */
void FSMC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 

	RCC_AHB1PeriphClockCmd(DB0_GPIO_CLK | DB1_GPIO_CLK | DB2_GPIO_CLK | DB3_GPIO_CLK, ENABLE);
	RCC_AHB1PeriphClockCmd(DB4_GPIO_CLK | DB5_GPIO_CLK | DB6_GPIO_CLK | DB7_GPIO_CLK, ENABLE);
	RCC_AHB1PeriphClockCmd(FSMC_RD_GPIO_CLK | FSMC_WR_GPIO_CLK | FSMC_NWAIT_GPIO_CLK | FSMC_NCE2_GPIO_CLK, ENABLE);
	RCC_AHB1PeriphClockCmd(FSMC_A16_GPIO_CLK | FSMC_A17_GPIO_CLK | FSMC_CLK_GPIO_CLK, ENABLE);
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
	
	/* GPIOD Source configuration  DB0->DB3 */
  GPIO_PinAFConfig(DB0_GPIO_PORT, DB0_GPIO_SOURCE, DB0_GPIO_AF);
  GPIO_PinAFConfig(DB1_GPIO_PORT, DB1_GPIO_SOURCE, DB1_GPIO_AF);
	GPIO_PinAFConfig(DB2_GPIO_PORT, DB2_GPIO_SOURCE, DB2_GPIO_AF);
  GPIO_PinAFConfig(DB3_GPIO_PORT, DB3_GPIO_SOURCE, DB3_GPIO_AF);
	/* GPIOE Source configuration  DB4->DB7 */	
  GPIO_PinAFConfig(DB4_GPIO_PORT, DB4_GPIO_SOURCE, DB4_GPIO_AF);
  GPIO_PinAFConfig(DB5_GPIO_PORT, DB5_GPIO_SOURCE, DB5_GPIO_AF);
  GPIO_PinAFConfig(DB6_GPIO_PORT, DB6_GPIO_SOURCE, DB6_GPIO_AF);
  GPIO_PinAFConfig(DB7_GPIO_PORT, DB7_GPIO_SOURCE, DB7_GPIO_AF);
	/* GPIOD configuration  RD and WR */	
  GPIO_PinAFConfig(FSMC_RD_GPIO_PORT, FSMC_RD_GPIO_SOURCE, FSMC_RD_GPIO_AF);
  GPIO_PinAFConfig(FSMC_WR_GPIO_PORT, FSMC_WR_GPIO_SOURCE, FSMC_WR_GPIO_AF);
	/* GPIOD configuration, Only used for NAND */	  
  GPIO_PinAFConfig(FSMC_NWAIT_GPIO_PORT, FSMC_NWAIT_GPIO_SOURCE, FSMC_NWAIT_GPIO_AF);   // FSMC_NWAIT
  GPIO_PinAFConfig(FSMC_NCE2_GPIO_PORT, FSMC_NCE2_GPIO_SOURCE, FSMC_NCE2_GPIO_AF);   // FSMC_NCE2
  GPIO_PinAFConfig(FSMC_A16_GPIO_PORT,FSMC_A16_GPIO_SOURCE, FSMC_A16_GPIO_AF);  // FSMC_A16 CLE
  GPIO_PinAFConfig(FSMC_A17_GPIO_PORT,FSMC_A17_GPIO_SOURCE, FSMC_A17_GPIO_AF);  // FSMC_A17 ALE
	
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd= GPIO_PuPd_UP;
	/* D0->D3, pin configuration and PD4-RD , PD5-WR, PD13-LCD_RS */
	//---------------------DB0-----------DB1-----------DB2----------DB3---------FSMC_RD-----FSMC_WR--------//
	GPIO_InitStructure.GPIO_Pin = DB0_PIN | DB1_PIN | DB2_PIN | DB3_PIN | FSMC_RD_PIN | FSMC_WR_PIN;
	GPIO_Init(DB0_GPIO_PORT, &GPIO_InitStructure);
	/* D4->D7 pin configuration*/
	//---------------------DB4-----------DB5----------DB6----------DB7-----------------//
	GPIO_InitStructure.GPIO_Pin = DB4_PIN | DB5_PIN | DB6_PIN | DB7_PIN;
  GPIO_Init(DB4_GPIO_PORT, &GPIO_InitStructure);
	
	//---------------------NCE2---------A16_CLE------A17_ALE------------------------------//
	GPIO_InitStructure.GPIO_Pin = FSMC_NCE2_PIN | FSMC_A16_PIN | FSMC_A17_PIN;
  GPIO_Init(FSMC_NCE2_GPIO_PORT, &GPIO_InitStructure);
	
	/*  GPIO Configuration, FSMC_NWAIT */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	//------------------------------NWAIT--------------------------//
	GPIO_InitStructure.GPIO_Pin = FSMC_NWAIT_PIN; 
  GPIO_Init(FSMC_NWAIT_GPIO_PORT, &GPIO_InitStructure);
	
	/*  GPIO Configuration, FSMC_CLK */    
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	//---FSMC_NAND-------------------CLK--------------------------//	
	GPIO_InitStructure.GPIO_Pin = FSMC_CLK_PIN; 
  GPIO_Init(FSMC_CLK_GPIO_PORT, &GPIO_InitStructure);
}

void FSMC_NAND_Config(void)
{
  FSMC_NANDInitTypeDef FSMC_NANDInitStructure;
  FSMC_NAND_PCCARDTimingInitTypeDef  p;

/*------------- FSMC NAND Configuration for K9F2G08U0B ----------------------------------------*/
	p.FSMC_SetupTime  = 0x05;		//0x01 //0x12
	p.FSMC_WaitSetupTime = 0x03;	//0x03	//0x0f
	p.FSMC_HoldSetupTime = 0x02;	//0x02	//0x08
	p.FSMC_HiZSetupTime= 0x01;		//0x01	//0x05

	FSMC_NANDInitStructure.FSMC_Bank = FSMC_Bank2_NAND;
	FSMC_NANDInitStructure.FSMC_Waitfeature = FSMC_Waitfeature_Enable;
	FSMC_NANDInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
	FSMC_NANDInitStructure.FSMC_ECC = FSMC_ECC_Enable;
	FSMC_NANDInitStructure.FSMC_ECCPageSize = FSMC_ECCPageSize_2048Bytes;
	//FSMC_NANDInitStructure.FSMC_AddressLowMapping = FSMC_AddressLowMapping_Direct;
	FSMC_NANDInitStructure.FSMC_TCLRSetupTime = 0x00;
	FSMC_NANDInitStructure.FSMC_TARSetupTime = 0x00;
	FSMC_NANDInitStructure.FSMC_CommonSpaceTimingStruct = &p;
	FSMC_NANDInitStructure.FSMC_AttributeSpaceTimingStruct = &p;
	FSMC_NANDInit(&FSMC_NANDInitStructure);

	/* FSMC NAND Bank Enable */
	FSMC_NANDCmd(FSMC_Bank2_NAND, ENABLE);
	/* Enables the FSMC NAND ECC feature */
	FSMC_NANDECCCmd(FSMC_Bank2_NAND, ENABLE);		
}

//-----------------------------------------------------------------------------
// NAND flash drive API
//-----------------------------------------------------------------------------
uint32_t FSMC_NAND_Reset(void)
{
//  NandFlashEn(); 
	*(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_RESET;
//  NandFlashDisable();
    return (NAND_READY);
}

uint32_t FSMC_NAND_ReadStatus(void)
{
  uint8_t data = 0x00, status = NAND_BUSY;

  /*!< Read status operation ------------------------------------ */
  *(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_STATUS;
  data = *(__IO uint8_t *)(Bank_NAND_ADDR);

  if((data & NAND_ERROR) == NAND_ERROR)
  {
    status = NAND_ERROR;
  } 
  else if((data & NAND_READY) == NAND_READY)
  {
    status = NAND_READY;
  }
  else
  {
    status = NAND_BUSY; 
  }
  
  return (status);
}

uint32_t FSMC_NAND_GetStatus(void)
{
  uint32_t timeout = 0x1000000, status = NAND_READY;

  status = FSMC_NAND_ReadStatus();

  /*!< Wait for a NAND operation to complete or a TIMEOUT to occur */
  while ((status != NAND_READY) &&( timeout != 0x00))
  {
     status = FSMC_NAND_ReadStatus();
     timeout --;
  }

  if(timeout == 0x00)
  {
    status =  NAND_TIMEOUT_ERROR;
  }

  /*!< Return the operation status */
  return (status);
}

uint32_t FlashBlockErase(flashInfo nandFlashInfo)
{
	uint32_t blockAddr;
	uint32_t flshStatus;
	
	blockAddr = nandFlashInfo.blockNo *(u32) PAGE_PER_BLOCK ;

	*(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = 0x60;

	*(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = (u8)(blockAddr);
	*(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = (u8)(blockAddr >> 8);
	*(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = (u8)(blockAddr >> 16 & 0x01);

	*(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = 0xd0;

	while(GPIO_ReadInputDataBit(FSMC_NWAIT_GPIO_PORT, FSMC_NWAIT_PIN) == 0);
	flshStatus = FSMC_NAND_GetStatus();
	
	return (flshStatus);
}

uint32_t FlashSecPageProgram(flashInfo nandFlashInfo, unsigned char * buff, unsigned int count)
{
	uint32_t status = NAND_READY;
	uint32_t rowAddr;
	uint16_t i;
	uint8_t  temp;
	
	rowAddr = nandFlashInfo.blockNo * (uint32_t)PAGE_PER_BLOCK + nandFlashInfo.pageNo;

	*(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = 0x80;

	*(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ((uint8_t)nandFlashInfo.pageAddr);
	*(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)(nandFlashInfo.pageAddr >> 8);
	*(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)(rowAddr);
	*(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)(rowAddr >> 8);
	*(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)(rowAddr >> 16 & 0x01);

	/* Write data */
	for(i=0; i < count; i++)
	{
		temp =  *(buff + i);
		*(vu8 *)(Bank_NAND_ADDR | DATA_AREA) = temp;
	}

	*(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = 0x10;

	while(GPIO_ReadInputDataBit(FSMC_NWAIT_GPIO_PORT, FSMC_NWAIT_PIN) == 0);
	/* Check status for successful operation */
	status = FSMC_NAND_GetStatus();

  return (status);
}

unsigned char FlashSec2SecCopy(flashInfo srcNandFlashInfo, flashInfo desNandFlashInfo)
{
	u32 rowAddr;
	uint32_t flshStatus;
	unsigned char bRet = FALSE;

	rowAddr = srcNandFlashInfo.blockNo *(u32) PAGE_PER_BLOCK + srcNandFlashInfo.pageNo;
	
//	NandFlashEn();

	*(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = 0x00;
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)(srcNandFlashInfo.pageAddr);
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)(srcNandFlashInfo.pageAddr >> 8);
	/*
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)(rowAddr);
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)(rowAddr >> 8);
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)(rowAddr >> 16 & 0x01);
	*/

	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)( ((uint8_t *)&rowAddr)[3]);
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)( ((uint8_t *)&rowAddr)[2]);
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)( ((uint8_t *)&rowAddr)[1]);
	*(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = 0x35;

	// Typical delay 25us
	while(GPIO_ReadInputDataBit(FSMC_NWAIT_GPIO_PORT, FSMC_NWAIT_PIN) == 0);
	
	rowAddr = desNandFlashInfo.blockNo *(u32) PAGE_PER_BLOCK + desNandFlashInfo.pageNo;
	*(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = 0x85;
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)(desNandFlashInfo.pageAddr);
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)(desNandFlashInfo.pageAddr >> 8);

	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)( ((uint8_t *)&rowAddr)[3]);
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)( ((uint8_t *)&rowAddr)[2]);
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)( ((uint8_t *)&rowAddr)[1]);

	*(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = 0x10;

	// Max delay 700us,typical value is 300us
	while(GPIO_ReadInputDataBit(FSMC_NWAIT_GPIO_PORT, FSMC_NWAIT_PIN) == 0);

	flshStatus = FSMC_NAND_GetStatus();

	if( (flshStatus & 0x01)  == 0)
	{
		bRet = TRUE;
	}

//	NandFlashDisable();
	return bRet;
}

uint32_t FlashSecPageRead(flashInfo nandFlashInfo, unsigned char * buff,unsigned int count)
{
	uint32_t flashStatus;
	uint16_t i,itemp;
	uint32_t rowAddr;
	
	rowAddr = nandFlashInfo.blockNo * (uint32_t)PAGE_PER_BLOCK + nandFlashInfo.pageNo;

	*(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = 0x00;

	*(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)(nandFlashInfo.pageAddr);
	*(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)(nandFlashInfo.pageAddr >> 8);
	*(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)(rowAddr);
	*(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)(rowAddr >> 8);
	*(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)(rowAddr >> 16 & 0x01);

	*(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = 0x30;

	while(GPIO_ReadInputDataBit(FSMC_NWAIT_GPIO_PORT, FSMC_NWAIT_PIN) == 0);

	for(itemp = 0;itemp<500;itemp++)
	{;;}
	/* Get Data into Buffer */
	for(i = 0; i < count; i++)
	{
			buff[i]= *(vu8 *)(Bank_NAND_ADDR | DATA_AREA);
	}

  flashStatus = FSMC_NAND_GetStatus();

  return (flashStatus);
}

void FSMC_NAND_ReadID(NAND_IDTypeDef* NAND_ID)
{
	uint32_t data = 0;
  /* Send Command to the command area */
  *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_READID;    //Ð´¶ÁIDÃüÁî
  *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00;
   /* Sequence to read ID from NAND flash */
	 Delay_ms(0xff);
   data = *(vu32 *)(Bank_NAND_ADDR | DATA_AREA); //¶ÁID 0x1580DAEC
   NAND_ID->Maker_ID   = ADDR_1st_CYCLE (data);  //0xEC
   NAND_ID->Device_ID  = ADDR_2nd_CYCLE (data);  //0xDA
   NAND_ID->Third_ID   = ADDR_3rd_CYCLE (data);  //0x80
   NAND_ID->Fourth_ID  = ADDR_4th_CYCLE (data);  //0x15
   data = *(vu8 *)(Bank_NAND_ADDR | DATA_AREA);
   NAND_ID->Fifth_ID   = ADDR_1st_CYCLE (data);  //0x50
}

void Delay_ms(__IO uint32_t nCount)
{
  while(nCount--)
  {
  }
}
/*********************************************END OF FILE**********************/
