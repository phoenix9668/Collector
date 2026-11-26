/**
 * @file    crc32.h
 * @brief   CRC-32/ISO-HDLC (the zlib / Ethernet CRC-32) used by the collars'
 *          RF payloads and by B0 frames.
 *
 * On the target crc32_hw.c uses the CRC peripheral (only the radio task may
 * call it); host tests link crc32_sw.c. Both give zlib.crc32() results.
 */
#ifndef CRC32_H
#define CRC32_H

#include <stddef.h>
#include <stdint.h>

uint32_t crc32_compute(const uint8_t *data, size_t len);

#endif /* CRC32_H */
