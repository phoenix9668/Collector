/**
 * @file    nvm.c
 * @brief   Data EEPROM word access (see nvm.h).
 */
#include "nvm.h"

#include "main.h"

bool nvm_read_word(uint32_t offset, uint32_t *value)
{
    if (!nvm_offset_valid(offset))
    {
        return false;
    }
    *value = *(const volatile uint32_t *)(DATA_EEPROM_BASE + offset);
    return true;
}

bool nvm_write_word(uint32_t offset, uint32_t value)
{
    if (!nvm_offset_valid(offset))
    {
        return false;
    }
    uint32_t address = DATA_EEPROM_BASE + offset;

    if (HAL_FLASHEx_DATAEEPROM_Unlock() != HAL_OK)
    {
        return false;
    }
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_WRPERR | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_RDERR |
                           FLASH_FLAG_FWWERR | FLASH_FLAG_NOTZEROERR);
    bool ok = HAL_FLASHEx_DATAEEPROM_Erase(address) == HAL_OK;
    if (ok)
    {
        HAL_FLASHEx_DATAEEPROM_EnableFixedTimeProgram();
        ok = HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, address, value) == HAL_OK;
        HAL_FLASHEx_DATAEEPROM_DisableFixedTimeProgram();
    }
    (void)HAL_FLASHEx_DATAEEPROM_Lock();
    return ok;
}
