/**
  ******************************************************************************
  * @file    sgm58031.c
  * @author  phoenix
  * @version V1.0.0
  * @date    20-April-2022
  * @brief   This module contains the low level operations for SGM58031
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "i2c.h"
#include "sgm58031.h"

/** @addtogroup STM32L0xx_HAL_Examples
  * @{
  */

/** @addtogroup SMBUS_SGM58031
  * @{
  */

/** @defgroup SGM58031
  * @{
  */

/** @defgroup SGM58031_Private_TypesDefinitions
  * @{
  */
SGM58031_InitTypeDef SGM58031_InitStructure;

/**
  * @}
  */

/** @defgroup SGM58031_Private_Defines
  * @{
  */

/**
  * @}
  */

/** @defgroup SGM58031_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup SGM58031_Private_Variables
  * @{
  */

/**
  * @}
  */

/** @defgroup SGM58031_Private_FunctionPrototypes
  * @{
  */

/**
  * @}
  */

/** @defgroup SGM58031_Private_Functions
  * @{
  */

/**
  * @brief  Configure SGM58031 Configuration Register.
  * @param
  * @param
  * @retval
  */
void SGM58031_ConfigReg(OS_SelTypeDef OS_Sel, MUX_SelTypeDef MUX_Sel, PGA_SelTypeDef PGA_Sel, MODE_SelTypeDef MODE_Sel, DR_SelTypeDef DR_Sel,
                        COMP_MODE_SelTypeDef COMP_MODE_Sel, COMP_POL_SelTypeDef COMP_POL_Sel, COMP_LAT_SelTypeDef COMP_LAT_Sel,
                        COMP_QUE_SelTypeDef COMP_QUE_Sel, DR_SEL_SelTypeDef DR_SEL_Sel)
{
    /*## Configure the Configuration Register of Analog-to-Digital Converter ###################################*/
    SGM58031_InitStructure.Config_Register.OS = OS_Sel;
    SGM58031_InitStructure.Config_Register.MUX = MUX_Sel;
    SGM58031_InitStructure.Config_Register.PGA = PGA_Sel;
    SGM58031_InitStructure.Config_Register.MODE = MODE_Sel;
    SGM58031_InitStructure.Config_Register.DR = DR_Sel;
    SGM58031_InitStructure.Config_Register.COMP_MODE = COMP_MODE_Sel;
    SGM58031_InitStructure.Config_Register.COMP_POL = COMP_POL_Sel;
    SGM58031_InitStructure.Config_Register.COMP_LAT = COMP_LAT_Sel;
    SGM58031_InitStructure.Config_Register.COMP_QUE = COMP_QUE_Sel;

    /*## Configure the Configuration1 Register of Analog-to-Digital Converter ###################################*/
    SGM58031_InitStructure.Config1_Register.DR_SEL = DR_SEL_Sel;

    SGM58031_InitStructure.Config_Register_Value = (SGM58031_InitStructure.Config_Register.OS << 15 & 0x8000) | (SGM58031_InitStructure.Config_Register.MUX << 12 & 0x7000)
            | (SGM58031_InitStructure.Config_Register.PGA << 9 & 0x0E00) | (SGM58031_InitStructure.Config_Register.MODE << 8 & 0x0100)
            | (SGM58031_InitStructure.Config_Register.DR << 5 & 0x00E0) | (SGM58031_InitStructure.Config_Register.COMP_MODE << 4 & 0x0010)
            | (SGM58031_InitStructure.Config_Register.COMP_POL << 3 & 0x0008) | (SGM58031_InitStructure.Config_Register.COMP_LAT << 2 & 0x0004)
            | SGM58031_InitStructure.Config_Register.COMP_QUE;
    SGM58031_InitStructure.Low_Thresh_Register = LOW_THRESH_REGISTER;
    SGM58031_InitStructure.High_Thresh_Register = HIGH_THRESH_REGISTER;
    SGM58031_InitStructure.Config1_Register_Value = (SGM58031_InitStructure.Config1_Register.DR_SEL << 7 & 0x0080);

    // debug_printf("Config_Register_Value = %x\n",SGM58031_InitStructure.Config_Register_Value);
    // debug_printf("Low_Thresh_Register = %x\n",SGM58031_InitStructure.Low_Thresh_Register);
    // debug_printf("High_Thresh_Register = %x\n",SGM58031_InitStructure.High_Thresh_Register);
    // debug_printf("Config1_Register_Value = %x\n",SGM58031_InitStructure.Config1_Register_Value);
}

/**
  * @brief  Set SGM58031 Initialization.
  * @param  pInitStruct: pointer to a SGM58031_InitTypeDef structure
  *         that contains the configuration setting for the SGM58031.
  * @retval SGM58031 status
  */
SGM58031_StatusTypeDef SGM58031_Init(uint8_t DeviceAddr)
{
    uint16_t tmp = 0;

    /*##-1- Initialise the SGM58031 peripheral ####################################*/
    SGM58031_ConfigReg(Start_Single_Conversion, AINP_AIN2_AND_AINN_AIN3, FS_4_096V, Single_Shot_Mode, DR_800Hz_960Hz, Traditional_Comparator, Active_Low, Non_Latching, Disable_Comparator, DR_SEL0);

    /*##-2- Read_CHIP_ID ############################*/
    tmp = SGM58031_ReadReg(DeviceAddr, SGM58031_CHIP_ID_REG);
    printf("######################################################################\n");
    printf("#----------------------------CHIP_ID = %x----------------------------#\n", tmp);

    if(tmp != 0x80)
        return SGM58031_ERROR;

    /*##-3- Set the Configuration Register #####################################*/
    if( SGM58031_WriteReg(DeviceAddr, SGM58031_CONF_REG, SGM58031_InitStructure.Config_Register_Value) != HAL_OK)
        return SGM58031_ERROR;

    /*##-4- Read the Configuration Register #####################################*/
    tmp = SGM58031_ReadReg(DeviceAddr, SGM58031_CONF_REG);
    printf("#--------------------------CONF_REG = %x---------------------------#\n", tmp);

    if((tmp & 0x7FFF) != (SGM58031_InitStructure.Config_Register_Value & 0x7FFF))
        return SGM58031_ERROR;

    /*##-5- Set the Configuration1 Register #####################################*/
    if( SGM58031_WriteReg(DeviceAddr, SGM58031_CONF1_REG, SGM58031_InitStructure.Config1_Register_Value) != HAL_OK)
        return SGM58031_ERROR;

    /*##-6- Read the Configuration1 Register #####################################*/
    tmp = SGM58031_ReadReg(DeviceAddr, SGM58031_CONF1_REG);
    printf("#----------------------------CONF1_REG = %x---------------------------#\n", tmp);

    if(tmp != SGM58031_InitStructure.Config1_Register_Value)
        return SGM58031_ERROR;

    /*##-7- Set the Low Thresh #####################################*/
    if( SGM58031_WriteReg(DeviceAddr, SGM58031_LOW_THRESH_REG, SGM58031_InitStructure.Low_Thresh_Register) != HAL_OK)
        return SGM58031_ERROR;

    /*##-8- read the Low Thresh Register #####################################*/
    tmp = SGM58031_ReadReg(DeviceAddr, SGM58031_LOW_THRESH_REG);
    printf("#---------------------Low_Thresh_Register = %x---------------------#\n", tmp);

    if(tmp != SGM58031_InitStructure.Low_Thresh_Register)
        return SGM58031_ERROR;

    /*##-9- Set the High Thresh ######################################*/
    if( SGM58031_WriteReg(DeviceAddr, SGM58031_HIGH_THRESH_REG, SGM58031_InitStructure.High_Thresh_Register) != HAL_OK)
        return SGM58031_ERROR;

    /*##-10- read the High Thresh Register #####################################*/
    tmp = SGM58031_ReadReg(DeviceAddr, SGM58031_HIGH_THRESH_REG);
    printf("#--------------------High_Thresh_Register = %x---------------------#\n", tmp);

    if(tmp != SGM58031_InitStructure.High_Thresh_Register)
        return SGM58031_ERROR;

    printf("#--------------------sgm58031 initial completed~!--------------------#\n");
    printf("######################################################################\n");
    /* Return status OK*/
    return SGM58031_OK;
}

/**
  * @brief  Read Register value of the Analog-to-Digital Converter.
  * @param  DeviceAddr : Device ID address.
  * @param  regaddr : Register address.
  * @retval ADC measured
  */
uint16_t SGM58031_ReadReg(uint8_t DeviceAddr, uint8_t regaddr)
{
    uint8_t bufferrx[2] = {0, 0};
    uint16_t tmp = 0;

    /*##-1- Send Read Command of the Analog-to-Digital Converter ####################################*/
    if( HAL_I2C_Master_Transmit(&hi2c1, DeviceAddr, &regaddr, 1, 1000) != HAL_OK)
        return SGM58031_ERROR;

    /*##-2- Retrieve Data Byte of the Analog-to-Digital Converter ###################################*/
    if( HAL_I2C_Master_Receive(&hi2c1, DeviceAddr + 1, bufferrx, 2, 1000) != HAL_OK)
        return SGM58031_ERROR;

    tmp = (uint16_t)(bufferrx[0] << 8);
    tmp |= bufferrx[1];

    /* Return Analog-to-Digital Converter value */
    return (uint16_t)tmp;
}

/**
  * @brief  Write Register value of the Analog-to-Digital Converter.
  * @param  DeviceAddr : Device ID address.
  * @param  regaddr : Register address.
  * @param  regvalue : Register value.
  * @retval SGM58031 status
  */
SGM58031_StatusTypeDef SGM58031_WriteReg(uint8_t DeviceAddr, uint8_t regaddr, uint16_t regvalue)
{
    uint8_t rBuffer[3] = {0, 0, 0};

    rBuffer[0] = regaddr;
    rBuffer[1] = (uint8_t)(regvalue >> 8);
    rBuffer[2] = (uint8_t)regvalue;

    /*##-1- Send Write Command and date of the Analog-to-Digital Converter ####################################*/
    if( HAL_I2C_Master_Transmit(&hi2c1, DeviceAddr, rBuffer, 3, 1000) != HAL_OK)
        return SGM58031_ERROR;

    /* Return Analog-to-Digital Converter Status */
    return SGM58031_OK;
}

/**
  * @brief  SGM58031 error callbacks.
  * @param  Error : Analog-to-Digital Converter Error status.
  * @retval None
  */
__weak void SGM58031_ErrorCallback(uint16_t Error)
{
    /* NOTE : This function Should not be modified, when the callback is needed,
              the SGM58031_ErrorCallback could be implemented in the user file
     */
    Error_Handler();
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
