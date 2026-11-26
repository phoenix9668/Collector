/**
 * @file    crc32_hw.c
 * @brief   CRC-32 on the STM32 CRC unit.
 *
 * Collector.ioc sets the unit to the default polynomial 0x04C11DB7, initial
 * value 0xFFFFFFFF, byte-reflected input and reflected output. Inverting the
 * result gives CRC-32/ISO-HDLC. Not reentrant: radio task only.
 */
#include "crc32.h"

#include "crc.h"

uint32_t crc32_compute(const uint8_t *data, size_t len)
{
    /* With CRC_INPUTDATA_FORMAT_BYTES the HAL reads the buffer byte-wise. */
    return ~HAL_CRC_Calculate(&hcrc, (uint32_t *)(uintptr_t)data, (uint32_t)len);
}
