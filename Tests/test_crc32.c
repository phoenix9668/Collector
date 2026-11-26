/**
 * @file    test_crc32.c
 * @brief   CRC-32/ISO-HDLC check values (zlib.crc32).
 */
#include <stdint.h>

#include "check.h"
#include "crc32.h"

void test_crc32(void)
{
    CHECK_EQ(crc32_compute((const uint8_t *)"123456789", 9), 0xCBF43926u);
    CHECK_EQ(crc32_compute((const uint8_t *)"", 0), 0x00000000u);
    CHECK_EQ(crc32_compute((const uint8_t *)"Heartbeat", 9), 0x888C6F7Au); /* zlib.crc32(b"Heartbeat") */
}
