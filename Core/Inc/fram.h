/********************************************************************************
 Author : CAC (China Applications Support Team) 

 Date :   May, 2014

 File name :  bsp_fram.h 

 Description :	 FM25L256 Registers                 

 Hardware plateform : 	Main_Board
********************************************************************************/
#ifndef _FM25L256DRIVER_H_
#define _FM25L256DRIVER_H_  

#include "main.h"

/* ------- op-codes ------- */
#define FRAM_WREN			  0x06	//Set Write Enable Latch
#define FRAM_WRDI			  0x04	//Write Disable
#define FRAM_RDSR				0x05	//Read Status Register
#define FRAM_WRSR				0x01	//Write Status Register
#define FRAM_READ				0x03	//Read Memory Data
#define FRAM_WRITE			0x02	//Write Memory Data

#define FRAM_DATA_LENGTH	16	// buffer length
#define RFID_SUM    		  16  //2048  // FM25L256 Organized as 32,768 x 8 bits 

extern __IO uint32_t CollectorID;
extern __IO uint8_t RFID_init[RFID_SUM][FRAM_DATA_LENGTH];

void FM25L256WriteEnable(void);
void FM25L256WriteDisable(void);
uint8_t FM25L256ReadStatusRegister(void);
void FM25L256WriteStatusRegister(uint8_t SendValue);
void FM25L256Read(uint16_t Address, uint16_t NumberofData, uint8_t *Data);
void FM25L256Write(uint16_t Address, uint16_t NumberofData, uint8_t *Data);
void FramCtrl(uint8_t *command);
void InitFramInfo(void);

#endif
