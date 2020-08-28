/**
  ******************************************************************************
  * @file    cc1101.h
  * @author  phoenix
  * @version V1.0.0
  * @date    20-October-2017
  * @brief   This file provides set of cc1101 to manage RF functions
  *          available on STM32F4-Discovery Kit from STMicroelectronics.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */ 

#ifndef _CC1101_H_
#define _CC1101_H_

#include "cc1101_reg.h"
#include "main.h"

#define Delay_TimeOut			500000

/*===========================================================================
----------------------------------macro definitions--------------------------
============================================================================*/
typedef enum { TX_MODE, RX_MODE, IDLE_MODE } TRMODE;
typedef enum { BROAD_ALL, BROAD_NO, BROAD_0, BROAD_0AND255 } ADDR_MODE;
typedef enum { BROADCAST, ADDRESS_CHECK} TX_DATA_MODE;

/*===========================================================================
-------------------------------------exported APIs---------------------------
============================================================================*/

/*read a byte from the specified register*/
uint8_t CC1101ReadReg(uint8_t addr);

/*Read a status register*/
uint8_t CC1101ReadStatus(uint8_t addr);

/*Set the device as TX mode or RX mode*/
void CC1101SetTRMode(TRMODE mode, FunctionalState NewState);

/*Write a command byte to the device*/
void CC1101WriteCmd(uint8_t command);

/*Set the CC1101 into IDLE mode*/
void CC1101SetIdle(void);

/*Send a packet*/
void CC1101SendPacket(uint8_t *txbuffer, uint8_t size, TX_DATA_MODE mode);

/*Set the address and address mode of the CC1101*/
void CC1101SetAddress(uint8_t address, ADDR_MODE AddressMode);

/*Set the SYNC bytes of the CC1101*/
void CC1101SetSYNC(uint16_t sync);

/*Receive a packet*/
uint8_t CC1101RecPacket(uint8_t *rxBuffer, uint8_t *addr, uint8_t *rssi);

/*Initialize the WOR function of CC1101*/
void CC1101WORInit(void);

/*Set the device as the WOR mode*/
void CC1101SetWORMode(void);

/*Initialize the CC1101, User can modify it*/
void CC1101Init(uint8_t addr, uint16_t sync);

/*Initialize the CC1101, Configure addr&sync&mode*/
void RFIDInitial(uint8_t addr, uint16_t sync, TRMODE mode);

/*read some bytes from the rigisters continously*/
void CC1101ReadMultiReg(uint8_t addr, uint8_t *buff, uint8_t size);

/*write a byte to the specified register*/
void CC1101WriteReg(uint8_t addr, uint8_t value);

/*flush the TX buffer of CC1101*/
void CC1101ClrTXBuff(void);

/*flush the RX buffer of CC1101*/
void CC1101ClrRXBuff(void);

/*get received count of CC1101*/
uint8_t CC1101GetRXCnt(void);

/*reset the CC1101 device*/
void CC1101Reset(void);

/*write some bytes to the specified register*/
void CC1101WriteMultiReg(uint8_t addr, uint8_t *buff, uint8_t size);

/*write one byte through SPI,in the meantime read one byte*/
extern uint8_t SPI_ExchangeByte(uint8_t input);

/*Read the RSSI value in rx*/
int16_t CC1101ReadRSSI(void);

/*Calc the RSSI value to RSSI dBm*/
int16_t CC1101CalcRSSI_dBm(uint8_t rssi_dec);

#endif // _CC1101_H_

/******************* END OF FILE ******************/
