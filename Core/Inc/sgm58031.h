/**
  ******************************************************************************
  * @file    sgm58031.h
  * @author  phoenix
  * @version V1.0.0
  * @date    20-April-2022
  * @brief   This module contains the low level operations for SGM58031
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */ 
  

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SGM58031_H
#define __SGM58031_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup SGM58031
  * @{
  */
#define SGM58031_ADDR                0x90    /* SGM58031 Write Address */
#define LOW_THRESH_REGISTER          0x8000
#define HIGH_THRESH_REGISTER         0x7FFF

/** @defgroup SGM58031_Exported_Types
  * @{
  */ 
/** 
  * @brief  Analog-to-Digital Converter driver structure definition  
  */
/** @defgroup Working Status/Single-Shot Conversion Start
  * @{
  */
typedef enum
{
  No_Effect                        = ((uint16_t)0x00),    /*!< 0 = No effect */
  Start_Single_Conversion          = ((uint16_t)0x01),    /*!< 1 = Start a single conversion (when in single-shot mode) */
                                  
}OS_SelTypeDef;

/** @defgroup Input Multiplexer (MUX) Configuration 
  * @}
  */
typedef enum
{
    AINP_AIN0_AND_AINN_AIN1          = ((uint16_t)0x00),    /*!< 000 = AINP = AIN0 and AINN = AIN1 (default) */
    AINP_AIN0_AND_AINN_AIN3          = ((uint16_t)0x01),    /*!< 001 = AINP = AIN0 and AINN = AIN3 */
    AINP_AIN1_AND_AINN_AIN3          = ((uint16_t)0x02),    /*!< 010 = AINP = AIN1 and AINN = AIN3 */
    AINP_AIN2_AND_AINN_AIN3          = ((uint16_t)0x03),    /*!< 011 = AINP = AIN2 and AINN = AIN3 */
    AINP_AIN0_AND_AINN_GND           = ((uint16_t)0x04),    /*!< 100 = AINP = AIN0 and AINN = GND */
	AINP_AIN1_AND_AINN_GND           = ((uint16_t)0x05),    /*!< 101 = AINP = AIN1 and AINN = GND */
	AINP_AIN2_AND_AINN_GND           = ((uint16_t)0x06),    /*!< 110 = AINP = AIN2 and AINN = GND */
	AINP_AIN3_AND_AINN_GND           = ((uint16_t)0x07),    /*!< 111 = AINP = AIN3 and AINN = GND */
	
}MUX_SelTypeDef;

/** @defgroup Programmable Gain Amplifier (PGA) Configuration
  * @{
  */
typedef enum
{
    FS_6_144V                        = ((uint16_t)0x00),    /*!< 000 = FS = +/-6.144V */
    FS_4_096V                        = ((uint16_t)0x01),    /*!< 001 = FS = +/-4.096V */
    FS_2_048V                        = ((uint16_t)0x02),    /*!< 010 = FS = +/-2.048V (default) */
    FS_1_024V                        = ((uint16_t)0x03),    /*!< 011 = FS = +/-1.024V */
    FS_0_512V                        = ((uint16_t)0x04),    /*!< 100 = FS = +/-0.512V */
	FS_0_256V                        = ((uint16_t)0x05),    /*!< 101 = FS = +/-0.256V */
	
}PGA_SelTypeDef;

/** @defgroup Device Operating Mode
  * @{
  */
typedef enum
{
    Continuous_Conversion_Mode       = ((uint16_t)0x00),    /*!< 0 = Continuous conversion mode  */
    Single_Shot_Mode                 = ((uint16_t)0x01),    /*!< 1 = Power-down single-shot mode (default) */
	
}MODE_SelTypeDef;

/** @defgroup Data Rate
  * @{
  */
typedef enum
{
	                                 	                     /*!<| DR[2:0] Bits        | DR_SEL Bit in Config1 Register | */
                                   	                     /*!<| in Config Register  | DR_SEL = 0     | DR_SEL = 1    | */
    DR_6_25Hz_7_5Hz                  = ((uint16_t)0x00),    /*!<| 000                 | 6.25Hz         | 7.5Hz         | */
    DR_12_5Hz_15Hz                   = ((uint16_t)0x01),    /*!<| 001                 | 12.5Hz         | 15Hz          | */
    DR_25Hz_30Hz                     = ((uint16_t)0x02),    /*!<| 010                 | 25Hz           | 30Hz          | */
    DR_50Hz_60Hz                     = ((uint16_t)0x03),    /*!<| 011                 | 50Hz           | 60Hz          | */
    DR_100Hz_120Hz                   = ((uint16_t)0x04),    /*!<| 100                 | 100Hz          | 120Hz         | */
    DR_200Hz_240Hz                   = ((uint16_t)0x05),    /*!<| 101                 | 200Hz          | 240Hz         | */
    DR_400Hz_480Hz                   = ((uint16_t)0x06),    /*!<| 110                 | 400Hz          | 480Hz         | */
    DR_800Hz_960Hz                   = ((uint16_t)0x07),    /*!<| 111                 | 800Hz          | 960HZ         | */
	
}DR_SelTypeDef;

/** @defgroup Comparator Mode 
  * @{
  */
typedef enum
{
    Traditional_Comparator           = ((uint16_t)0x00),    /*!< 0 = A traditional comparator with hysteresis (default) */
    Window_Comparator                = ((uint16_t)0x01),    /*!< 1 = A window comparator */
	
}COMP_MODE_SelTypeDef;

/** @defgroup Comparator Polarity 
  * @{
  */
typedef enum
{
    Active_Low                       = ((uint16_t)0x00),    /*!< 0 = Active low (default) */
    Active_High                      = ((uint16_t)0x01),    /*!< 1 = Active high */
	
}COMP_POL_SelTypeDef;

/** @defgroup Latching Comparator 
  * @{
  */
typedef enum
{
    Non_Latching                     = ((uint16_t)0x00),    /*!< 0 = Non-latching comparator (default) */
    Latching                         = ((uint16_t)0x01),    /*!< 1 = Latching comparator */
	
}COMP_LAT_SelTypeDef;

/** @defgroup Comparator Queue and Disable Function
  * @{
  */
typedef enum
{
    Assert_After_One_Conversion      = ((uint16_t)0x00),    /*!< 00 = Assert after one conversion */
    Assert_After_Two_Conversions     = ((uint16_t)0x01),    /*!< 01 = Assert after two conversions */
	Assert_After_Four_Conversions    = ((uint16_t)0x02),    /*!< 10 = Assert after four conversions */
	Disable_Comparator               = ((uint16_t)0x03),    /*!< 11 = Disable comparator (default) */
	
}COMP_QUE_SelTypeDef;

/** @defgroup DR_SEL
  * @{
  */
typedef enum
{
    DR_SEL0                          = ((uint16_t)0x00),    /*!< 0 = DR[2:0] = 000 ~ 111 for conversion rate of 6.25Hz, 12.5Hz, 25Hz, 50Hz, 100Hz, 200Hz, 400Hz and 800Hz (default)  */
    DR_SEL1                          = ((uint16_t)0x01),    /*!< 1 = DR[2:0] = 000 ~ 111 for conversion rate of 7.5Hz, 15Hz, 30Hz, 60Hz, 120Hz, 240Hz, 480Hz and 960Hz*/

}DR_SEL_SelTypeDef;	
	
typedef struct
{
	uint8_t OS;                   /* Working Status/Single-Shot Conversion Start 
                                   For a write status: 
                                   0 = No effect 
                                   1 = Start a single conversion (when in single-shot 
                                   mode)
 
                                   For a read status: 
                                   0 = The chip is doing a conversion 
                                   1 = The chip isn't doing a conversion */
    uint8_t MUX;                  /* Input Multiplexer (MUX) Configuration 
                                   000 = AINP = AIN0 and AINN = AIN1 (default) 
                                   001 = AINP = AIN0 and AINN = AIN3 
                                   010 = AINP = AIN1 and AINN = AIN3 
                                   011 = AINP = AIN2 and AINN = AIN3 
                                   100 = AINP = AIN0 and AINN = GND 
                                   101 = AINP = AIN1 and AINN = GND 
                                   110 = AINP = AIN2 and AINN = GND 
                                   111 = AINP = AIN3 and AINN = GND */

	uint8_t PGA;                  /* Programmable Gain Amplifier (PGA) Configuration 
                                   000 = FS = +/-6.144V (1) 
                                   001 = FS = +/-4.096V (1) 
                                   010 = FS = +/-2.048V (default) 
                                   011 = FS = +/-1.024V 
                                   100 = FS = +/-0.512V 
                                   101 = FS = +/-0.256V 
                                   110 = FS = +/-0.256V 
                                   111 = FS = +/-0.256V */
	
	uint8_t MODE;                 /* Device Operating Mode 
                                   0 = Continuous conversion mode 
                                   1 = Power-down single-shot mode (default)  */
																	 
	uint8_t DR;                   /* Data Rate
	                                 | DR[2:0] Bits        | DR_SEL Bit in Config1 Register |
                                   | in Config Register  | DR_SEL = 0     | DR_SEL = 1    |
                                   | 000                 | 6.25Hz         | 7.5Hz         |
                                   | 001                 | 12.5Hz         | 15Hz          |
                                   | 010                 | 25Hz           | 30Hz          |
                                   | 011                 | 50Hz           | 60Hz          |
                                   | 100                 | 100Hz          | 120Hz         |
                                   | 101                 | 200Hz          | 240Hz         |
                                   | 110                 | 400Hz          | 480Hz         |
                                   | 111                 | 800Hz          | 960HZ         | */
																	 
	uint8_t COMP_MODE;            /* Comparator Mode 
                                   0 = A traditional comparator with hysteresis (default) 
                                   1 = A window comparator */
																	 
	uint8_t COMP_POL;             /* Comparator Polarity 
	                                 0 = Active low (default) 
	                                 1 = Active high */
																	 
	uint8_t COMP_LAT;             /* Latching Comparator 
	                                 0 = Non-latching comparator (default) 
	                                 1 = Latching comparator */
																	 
	uint8_t COMP_QUE;             /* Comparator Queue and Disable Function 
	                                 00 = Assert after one conversion 
	                                 01 = Assert after two conversions 
	                                 10 = Assert after four conversions 
	                                 11 = Disable comparator (default) */																	 

} Config_Register_InitTypeDef;

typedef struct
{
    uint8_t PD;                   /* Writing '1' to PD powers down this part, and this PD bit is automatically cleared 
                                   internally. Another continuous/single conversion can be carried out again without 
                                   the need to clear this bit. */

	uint8_t DR_SEL;               /* 0 = DR[2:0] = 000 ~ 111 for conversion rate of 6.25Hz, 12.5Hz, 25Hz, 50Hz, 100Hz, 
                                   200Hz, 400Hz and 800Hz (default) 
                                   1 = DR[2:0] = 000 ~ 111 for conversion rate of 7.5Hz, 15Hz, 30Hz, 60Hz, 120Hz, 
                                   240Hz, 480Hz and 960Hz */
	
	uint8_t BURNOUT;              /* 0 = No current sourced (default) 
                                   1 = Source a pair of 2uA current to selected pair of AINs */
																	 
	uint8_t BUS_FLEX;             /* 0 = Disable leakage blocking circuit for the scenario that I2C bus voltage is lower 
                                   than VDD of the part. The I2C interface is still functional but VDD sees leakage when 
                                   VBUS < VDD - 0.3V (default) 
                                   1 = Bus voltage can be lower than VDD without causing leakage. The VDD range is 
                                   3V to 5.5V and the I2C bus voltage should be limited to 3V to 5.5V */
																	 
	uint8_t EXT_REF ;             /* 0 = None (default) 
                                   1 = Use AIN3 as external reference for ADC */													 

} Config1_Register_InitTypeDef;

typedef struct
{
	uint16_t                        Conversion_Register;       /*!< The ADC conversion result is 16-bit two's complement format (Read-Only) */
    Config_Register_InitTypeDef     Config_Register;           /*!< The configuration register (Read/Write) */
	uint16_t                        Config_Register_Value;
    uint16_t                        Low_Thresh_Register;       /*!< Low_Thresh Register (Read/Write) */
    uint16_t                        High_Thresh_Register;      /*!< High_Thresh Register (Read/Write) */
	Config1_Register_InitTypeDef    Config1_Register;          /*!< The configuration register (Read/Write) */
	uint16_t                        Config1_Register_Value;
	uint16_t                        Chip_ID_Register;          /*!< 16-Bit Chip_ID Register for Identifying Chip ID and Its Subversions (Read-Only) */
	uint16_t                        GN_Trim1_Register;         /*!< ADC  gain  coefficient  for  user  selecting  Config1  register  EXT_REF  bit  as  reference.  We  provide  a  default  value  and  user  is  
                                                                  responsible for writing proper value to the register if they want to compensate external reference error. This register does not take 
                                                                  effect when EXT_REF = 0 and internal reference is selected. */
	
} SGM58031_InitTypeDef;

/** @defgroup SGM58031_Private_TypesDefinitions
  * @{
  */
extern SGM58031_InitTypeDef SGM58031_InitStructure;

/** 
  * @brief  SGM58031 Status structures definition  
  */  
typedef enum 
{
  SGM58031_OK       = 0x00,
  SGM58031_ERROR    = 0x01,
  SGM58031_ALERT    = 0x02
} SGM58031_StatusTypeDef;

/** @defgroup SGM58031_Exported_Constants
  * @{
  */
/******************************************************************************/
/*************************** START REGISTER MAPPING  **************************/
/******************************************************************************/
/***************************** Read Access Only *******************************/
#define SGM58031_CONVERSION_REG       0x00  /*!< Conversion Register of SGM58031 */
#define SGM58031_CHIP_ID_REG          0x05  /*!< Chip_ID Register of SGM58031 */

/***************************** Read/Write Access ******************************/
#define SGM58031_CONF_REG             0x01  /*!< Config Register of SGM58031 */
#define SGM58031_LOW_THRESH_REG       0x02  /*!< Low_Thresh Register of SGM58031 */
#define SGM58031_HIGH_THRESH_REG      0x03  /*!< High_Thresh Register of SGM58031 */
#define SGM58031_CONF1_REG            0x04  /*!< Config1 Register of SGM58031 */
#define SGM58031_GN_TRIM1_REG         0x06  /*!< GN_Trim1 Register for EXT_REF */


/******************************************************************************/
/**************************** END REGISTER MAPPING  ***************************/
/******************************************************************************/

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup SGM58031_Exported_Functions
  * @{
  */
/* Sensor Configuration Functions */ 
void                   SGM58031_ConfigReg(OS_SelTypeDef OS_Sel, MUX_SelTypeDef MUX_Sel, PGA_SelTypeDef PGA_Sel, MODE_SelTypeDef MODE_Sel, DR_SelTypeDef DR_Sel, 
                                          COMP_MODE_SelTypeDef COMP_MODE_Sel, COMP_POL_SelTypeDef COMP_POL_Sel, COMP_LAT_SelTypeDef COMP_LAT_Sel, 
                                          COMP_QUE_SelTypeDef COMP_QUE_Sel, DR_SEL_SelTypeDef DR_SEL_Sel);
SGM58031_StatusTypeDef SGM58031_Init(uint8_t DeviceAddr);

/* Sensor Request Functions */
uint16_t               SGM58031_ReadReg(uint8_t DeviceAddr, uint8_t regaddr);
SGM58031_StatusTypeDef SGM58031_WriteReg(uint8_t DeviceAddr, uint8_t regaddr, uint16_t regvalue);

/* Sensor Callbacks */
__weak void            SGM58031_ErrorCallback(uint16_t Error);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
 }
#endif
  
#endif /* __SGM58031_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/ 
