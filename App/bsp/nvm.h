/**
 * @file    nvm.h
 * @brief   32-bit words in the STM32L051 data EEPROM (2 KB at 0x08080000).
 *
 * Offset 0x0000 holds the CollectorID (APP_NVM_COLLECTOR_ID_OFFSET).
 */
#ifndef NVM_H
#define NVM_H

#include <stdbool.h>
#include <stdint.h>

#define NVM_SIZE 2048u

static inline bool nvm_offset_valid(uint32_t offset)
{
    return (offset % 4u) == 0u && offset <= NVM_SIZE - 4u;
}

bool nvm_read_word(uint32_t offset, uint32_t *value);
bool nvm_write_word(uint32_t offset, uint32_t value);

#endif /* NVM_H */
