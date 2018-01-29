#ifndef __BSP_FSMC_H
#define	__BSP_FSMC_H

#include "stm32f4xx.h"

#define TRUE																						1
#define FALSE																					0
/* NAND Area definition  for STM3210E-EVAL Board RevD */
#define CMD_AREA                   							(uint32_t)(1<<16)  /* A16 = CLE  high */
#define ADDR_AREA                  							(uint32_t)(1<<17)  /* A17 = ALE high */
#define DATA_AREA                  							((uint32_t)0x00000000) 
#define Bank_NAND_ADDR     								Bank2_NAND_ADDR 
#define Bank2_NAND_ADDR    							((uint32_t)0x70000000)
#define PAGE_PER_BLOCK									64
#define BYTES_PER_PAGE										(2048 + 64)
/* NAND memory status */
#define NAND_VALID_ADDRESS         	((uint32_t)0x00000100)
#define NAND_INVALID_ADDRESS       ((uint32_t)0x00000200)
#define NAND_TIMEOUT_ERROR       	((uint32_t)0x00000400)
#define NAND_BUSY                  							((uint32_t)0x00000000)
#define NAND_ERROR                 					((uint32_t)0x00000001)
#define NAND_READY                 						((uint32_t)0x00000040)
#define NAND_CMD_READID            		((uint8_t)0x90)
#define NAND_CMD_STATUS            		((uint8_t)0x70)
#define NAND_CMD_RESET            	 		((uint8_t)0xFF)
/* FSMC NAND memory address computation */
#define ADDR_1st_CYCLE(ADDR)       	(uint8_t)((ADDR)& 0xFF)               /* 1st addressing cycle */
#define ADDR_2nd_CYCLE(ADDR)       (uint8_t)(((ADDR)& 0xFF00) >> 8)      /* 2nd addressing cycle */
#define ADDR_3rd_CYCLE(ADDR)       	(uint8_t)(((ADDR)& 0xFF0000) >> 16)   /* 3rd addressing cycle */
#define ADDR_4th_CYCLE(ADDR)				(uint8_t)(((ADDR)& 0xFF000000) >> 24) /* 4th addressing cycle */

typedef struct
{
  unsigned char Maker_ID;
  unsigned char Device_ID;
  unsigned char Third_ID;
  unsigned char Fourth_ID;
  unsigned char Fifth_ID; 
}NAND_IDTypeDef; 

typedef struct{
	unsigned int blockNo;
	unsigned char pageNo;
	unsigned int pageAddr;
}  flashInfo, *pFlashInfo;

//PIN define
/*******************************************************/
#define DB0_PIN																						GPIO_Pin_14
#define DB0_GPIO_PORT        										GPIOD                      
#define DB0_GPIO_CLK             									RCC_AHB1Periph_GPIOD
#define DB0_GPIO_SOURCE           						GPIO_PinSource14
#define DB0_GPIO_AF               										GPIO_AF_FSMC

#define DB1_PIN																						GPIO_Pin_15
#define DB1_GPIO_PORT        										GPIOD
#define DB1_GPIO_CLK             									RCC_AHB1Periph_GPIOD
#define DB1_GPIO_SOURCE           						GPIO_PinSource15
#define DB1_GPIO_AF               										GPIO_AF_FSMC

#define DB2_PIN																						GPIO_Pin_0
#define DB2_GPIO_PORT        										GPIOD
#define DB2_GPIO_CLK             									RCC_AHB1Periph_GPIOD
#define DB2_GPIO_SOURCE           						GPIO_PinSource0
#define DB2_GPIO_AF               										GPIO_AF_FSMC

#define DB3_PIN																						GPIO_Pin_1 
#define DB3_GPIO_PORT        										GPIOD                   
#define DB3_GPIO_CLK             									RCC_AHB1Periph_GPIOD
#define DB3_GPIO_SOURCE           						GPIO_PinSource1
#define DB3_GPIO_AF               										GPIO_AF_FSMC

#define DB4_PIN																						GPIO_Pin_7               
#define DB4_GPIO_PORT        										GPIOE                   
#define DB4_GPIO_CLK             									RCC_AHB1Periph_GPIOE
#define DB4_GPIO_SOURCE           						GPIO_PinSource7
#define DB4_GPIO_AF               										GPIO_AF_FSMC

#define DB5_PIN																						GPIO_Pin_8               
#define DB5_GPIO_PORT        										GPIOE        
#define DB5_GPIO_CLK             									RCC_AHB1Periph_GPIOE
#define DB5_GPIO_SOURCE           						GPIO_PinSource8
#define DB5_GPIO_AF               										GPIO_AF_FSMC

#define DB6_PIN																						GPIO_Pin_9                
#define DB6_GPIO_PORT        										GPIOE                     
#define DB6_GPIO_CLK             									RCC_AHB1Periph_GPIOE
#define DB6_GPIO_SOURCE           						GPIO_PinSource9
#define DB6_GPIO_AF               										GPIO_AF_FSMC

#define DB7_PIN																						GPIO_Pin_10                 
#define DB7_GPIO_PORT        										GPIOE                   
#define DB7_GPIO_CLK             									RCC_AHB1Periph_GPIOE
#define DB7_GPIO_SOURCE           						GPIO_PinSource10
#define DB7_GPIO_AF               										GPIO_AF_FSMC

#define FSMC_RD_PIN																	GPIO_Pin_4          
#define FSMC_RD_GPIO_PORT   							GPIOD
#define FSMC_RD_GPIO_CLK      							RCC_AHB1Periph_GPIOD
#define FSMC_RD_GPIO_SOURCE   					GPIO_PinSource4
#define FSMC_RD_GPIO_AF               				GPIO_AF_FSMC

#define FSMC_WR_PIN																	GPIO_Pin_5
#define FSMC_WR_GPIO_PORT   							GPIOD
#define FSMC_WR_GPIO_CLK      							RCC_AHB1Periph_GPIOD
#define FSMC_WR_GPIO_SOURCE   				GPIO_PinSource5
#define FSMC_WR_GPIO_AF               				GPIO_AF_FSMC

#define FSMC_NWAIT_PIN														GPIO_Pin_6              
#define FSMC_NWAIT_GPIO_PORT   				GPIOD
#define FSMC_NWAIT_GPIO_CLK      				RCC_AHB1Periph_GPIOD
#define FSMC_NWAIT_GPIO_SOURCE   	GPIO_PinSource6
#define FSMC_NWAIT_GPIO_AF               	GPIO_AF_FSMC

#define FSMC_NCE2_PIN															GPIO_Pin_7
#define FSMC_NCE2_GPIO_PORT   					GPIOD
#define FSMC_NCE2_GPIO_CLK      					RCC_AHB1Periph_GPIOD
#define FSMC_NCE2_GPIO_SOURCE   		GPIO_PinSource7
#define FSMC_NCE2_GPIO_AF               		GPIO_AF_FSMC

#define FSMC_A16_PIN																GPIO_Pin_11          
#define FSMC_A16_GPIO_PORT   							GPIOD
#define FSMC_A16_GPIO_CLK      							RCC_AHB1Periph_GPIOD
#define FSMC_A16_GPIO_SOURCE   				GPIO_PinSource11
#define FSMC_A16_GPIO_AF               				GPIO_AF_FSMC

#define FSMC_A17_PIN																GPIO_Pin_12
#define FSMC_A17_GPIO_PORT   							GPIOD
#define FSMC_A17_GPIO_CLK      							RCC_AHB1Periph_GPIOD
#define FSMC_A17_GPIO_SOURCE   				GPIO_PinSource12
#define FSMC_A17_GPIO_AF               				GPIO_AF_FSMC

#define FSMC_CLK_PIN																GPIO_Pin_3
#define FSMC_CLK_GPIO_PORT   						GPIOD
#define FSMC_CLK_GPIO_CLK      						RCC_AHB1Periph_GPIOD
/*******************************************************/

#define NandFlashEn() 																	GPIO_ResetBits(FSMC_NCE2_GPIO_PORT, FSMC_NCE2_PIN)
#define NandFlashDisable()													GPIO_SetBits(FSMC_NCE2_GPIO_PORT, FSMC_NCE2_PIN)
void FSMC_GPIO_Config(void);
void FSMC_NAND_Config(void);
void FSMC_NAND_ReadID(NAND_IDTypeDef* NAND_ID);
uint32_t FSMC_NAND_Reset(void);
uint32_t FSMC_NAND_ReadStatus(void);
uint32_t FSMC_NAND_GetStatus(void);
uint32_t FlashSecPageRead(flashInfo nandFlashInfo, unsigned char * buff,unsigned int count);
uint32_t FlashBlockErase(flashInfo nandFlashInfo);
uint32_t FlashSecPageProgram(flashInfo nandFlashInfo, unsigned char * buff, unsigned int count);
unsigned char FlashSec2SecCopy(flashInfo srcNandFlashInfo, flashInfo desNandFlashInfo);	
void Delay_ms(__IO uint32_t nCount);

#endif /* __LED_H */

