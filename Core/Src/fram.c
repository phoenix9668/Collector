/********************************************************************************
 Author : CAC (China Applications Support Team) 

 Date :   May, 2014

 File name :  FM25L256-G.c 

 Description :	 FM25L356 SPI communication driver                

 Hardware plateform : 	Main_Board
 
 Connection:
                 Main_Board       		FM25L256

                 PB.14:MISO,          	MISO
                 PB.13:SCLK,          	SCLK
                 PB.15:MOSI,          	MOSI
                 PB.12:GPIO as CS,       CS
********************************************************************************/

#include "fram.h"
#include "spi.h"
#include "gpio.h"

__IO uint32_t CollectorID;
__IO uint8_t RFID_init[RFID_SUM][FRAM_DATA_LENGTH] = {0};
uint8_t FRAM_Data[FRAM_DATA_LENGTH];

/*******************************************************************
  @brief void FM25L256WriteEnable()
         Set Write Enable Latch         
  @param none
  @return	none
*******************************************************************/
void FM25L256WriteEnable(void)
{
		uint8_t SendTemp[1];
		uint8_t ReceiveTemp[1];

		SendTemp[0] = FRAM_WREN;
    FRAM_CSN_LOW();              				//CS down
    SpiFunction(SendTemp, ReceiveTemp, 1, 0);
    FRAM_CSN_HIGH();										//CS up
}

/*******************************************************************
  @brief void FM25L256WriteDisable()
         Set Write Disable Latch         
  @param none
  @return	none
*******************************************************************/
void FM25L256WriteDisable(void)
{
		uint8_t SendTemp[1];
		uint8_t ReceiveTemp[1];

		SendTemp[0] = FRAM_WRDI;
    FRAM_CSN_LOW();              				//CS down
    SpiFunction(SendTemp, ReceiveTemp, 1, 0);
    FRAM_CSN_HIGH();										//CS up
}

/*******************************************************************
  @brief uint8_t FM25L256ReadStatusRegister()
         Read Status Register value from FM25L256         
  @param none
  @return   
         uint8_t ReceiveValue:  Read Status Register value
*******************************************************************/
uint8_t FM25L256ReadStatusRegister(void)
{
		uint8_t SendTemp[1];
		uint8_t ReceiveTemp[1];
		uint8_t ReceiveValue;

		SendTemp[0] = FRAM_RDSR;
    FRAM_CSN_LOW();              				//CS down  
    SpiFunction(SendTemp, ReceiveTemp, 1, 1);
    FRAM_CSN_HIGH();										//CS up
		ReceiveValue = ReceiveTemp[0];
    return(ReceiveValue);
}

/*******************************************************************
  @brief void FM25L256WriteStatusRegister(uint8_t SendValue)
         Write Status Register value to FM25L256         
  @param 
				 uint8_t SendValue: Value written to FM25L256 Status Register
  @return
         none
*******************************************************************/
void FM25L256WriteStatusRegister(uint8_t SendValue)
{
		uint8_t SendTemp[2];
		uint8_t ReceiveTemp[1];

		SendTemp[0] = FRAM_WRSR;
		SendTemp[1] = SendValue;
    FRAM_CSN_LOW();              				//CS down  
    SpiFunction(SendTemp, ReceiveTemp, 2, 0);
    FRAM_CSN_HIGH();										//CS up
}

/*******************************************************************
  @brief void FM25L256Read(uint16_t Address, uint16_t NumberofData, uint8_t *Data)
         Multibyte read from FM25L256
  @param
				 uint16_t Address:     	Data address
         uint16_t NumberofData: Data numbers to be read
         uint8_t *Data:     		Buffer save the read value
  @return
         none
*******************************************************************/
void FM25L256Read(uint16_t Address, uint16_t NumberofData, uint8_t *Data)
{
		uint8_t SendTemp[3];
	
		SendTemp[0] = FRAM_READ;
		SendTemp[1] = (uint8_t)(0xFF & Address>>8);
		SendTemp[2] = (uint8_t)(0xFF & Address);
    FRAM_CSN_LOW();         			//CS down
    SpiFunction(SendTemp, Data, 3, NumberofData);
    FRAM_CSN_HIGH();        			//CS up
}

/*******************************************************************
  @brief void FM25L256Write(uint16_t Address, uint16_t NumberofData, uint8_t *Data)
         Multibyte write to FM25L256
  @param
				 uint16_t Address:     	Data address
         uint16_t NumberofData: Data numbers to be read
         uint8_t *Data:     		Buffer save the read value
  @return
         none
*******************************************************************/
void FM25L256Write(uint16_t Address, uint16_t NumberofData, uint8_t *Data)
{
    uint8_t SendTemp[24+3];
    uint8_t ReceiveTemp[1];
    uint8_t Index;

    SendTemp[0] = FRAM_WRITE;                 //0x02: write register
    SendTemp[1] = (uint8_t)(0xFF & Address>>8);      	//address byte
		SendTemp[2] = (uint8_t)(0xFF & Address);					//address byte
    for (Index=0; Index<NumberofData; Index++)
    {
        SendTemp[3+Index] = *(Data + Index);
    }
		FRAM_CSN_LOW();														//CS down
    SpiFunction(SendTemp, ReceiveTemp, (3+NumberofData), 0);
    FRAM_CSN_HIGH();              						//CS up
}

/*******************************************************************
  @brief FRAMCtrl(uint8_t *command)
         through commamd control FM25L256
  @param
				 uint8_t *command:     	control command
  @return
         none
*******************************************************************/
void FramCtrl(uint8_t *command)
{
	uint16_t address;
	
	address = (uint16_t)(0xFF00 & command[3]<<8)+(uint16_t)(0x00FF & command[4]);
	for(uint16_t i = 0; i < FRAM_DATA_LENGTH; i++)
	{
		FRAM_Data[i] = command[i+5];
	}
	if(command[2] == 0x01)//write
	{
		FM25L256WriteEnable();
		FM25L256Write(address, FRAM_DATA_LENGTH, FRAM_Data);
		printf("Write Memory Complete\r\n");
	}
	else if(command[2] == 0x02)//read
	{
		FM25L256Read(address, FRAM_DATA_LENGTH, FRAM_Data);
		for(uint16_t i = 0; i < FRAM_DATA_LENGTH; i++)
		{
			printf("%02x",FRAM_Data[i]);
		}
		printf("\nRead Memory Complete\r\n");
	}
	else if(command[2] == 0x03)//erase
	{
		for(uint16_t i = 0; i < FRAM_DATA_LENGTH; i++)
		{
			FRAM_Data[i] = 0;
		}
		for(uint16_t i = 0; i < RFID_SUM; i++)
		{
			address = FRAM_DATA_LENGTH*i;
			FM25L256WriteEnable();
			FM25L256Write(address, FRAM_DATA_LENGTH, FRAM_Data);
		}
		printf("Erase All Memory Complete\r\n");
	}
	else if(command[2] == 0x04)//print info
	{
		InitFramInfo();
		printf("CollectorID:%x\n",CollectorID);
		for(uint16_t i = 0; i < RFID_SUM; i++)
		{
			for(uint8_t j = 0; j < FRAM_DATA_LENGTH; j++)
			{
				printf("%02x",RFID_init[i][j]);
			}
			printf("\n");
			HAL_Delay(1);
		}
		printf("\nPrint Memory Complete\r\n");
	}

}

/*******************************************************************
  @brief InitFramInfo(void)
         initial ID infomation
  @param
				 none
  @return
         none
*******************************************************************/
void InitFramInfo(void)
{
	uint16_t address=0;

	for(uint16_t i = 0; i < RFID_SUM; i++)
	{
		address = FRAM_DATA_LENGTH*i;
		FM25L256Read(address, FRAM_DATA_LENGTH, FRAM_Data);
		for(uint8_t j = 0; j < FRAM_DATA_LENGTH; j++)
		{
			RFID_init[i][j] = FRAM_Data[j];
		}
	}
	CollectorID = (uint32_t)(0xFF000000 & RFID_init[0][0]<<24)+(uint32_t)(0x00FF0000 & RFID_init[0][1]<<16)+(uint32_t)(0x0000FF00 & RFID_init[0][2]<<8)+(uint32_t)(0x000000FF & RFID_init[0][3]);
}

