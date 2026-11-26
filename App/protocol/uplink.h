/**
 * @file    uplink.h
 * @brief   Frames the Collector sends on USART1 (and B0 also on LPUART1).
 *
 * B0, one per valid collar packet. L = RF payload length including the
 * collar's 4-byte CRC-32; the frame is L + 15 bytes (206 for L = 191):
 *
 *   0..3        CollectorID, MSB first
 *   4           0xB0
 *   5..L        RF payload without its CRC (L - 4 bytes)
 *   L+1         RSSI, raw CC1101 register value
 *   L+2..L+5    year (0..99), month, date, weekday
 *   L+6..L+8    hours, minutes, seconds
 *   L+9..L+12   CRC-32 of bytes 0..L+8, MSB first
 *   L+13..L+14  0x0D 0x0A
 *
 * B2, supply voltages from the SGM58031, 9 bytes, no CRC, no tail:
 *
 *   0..3  CollectorID   4  0xB2
 *   5..6  battery code (AIN2-AIN3), MSB first   7..8  power code (AIN0-AIN1)
 *
 * The station converts a code to volts as code / 32768 * 4.096 * 11 (battery)
 * or * 21 (power).
 */
#ifndef UPLINK_H
#define UPLINK_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "collector_time.h"

#define UPLINK_ID_LEN       4u
#define UPLINK_FN_B0        0xB0u
#define UPLINK_FN_B2        0xB2u
#define UPLINK_RF_CRC_LEN   4u
#define UPLINK_B0_OVERHEAD  15u /* frame length = RF payload length + this */
#define UPLINK_B2_LEN       9u

/* True when the last 4 bytes (MSB first) are the CRC-32 of the rest. */
bool uplink_rf_crc_ok(const uint8_t *rf, size_t rf_len);

/* Returns the frame length, or 0 if rf_len < 4 or the frame does not fit. */
size_t uplink_build_b0(uint8_t *out, size_t cap, const uint8_t id[UPLINK_ID_LEN], const uint8_t *rf,
                       size_t rf_len, uint8_t rssi_raw, const collector_time_t *time);

/* Returns UPLINK_B2_LEN, or 0 if cap is too small. */
size_t uplink_build_b2(uint8_t *out, size_t cap, const uint8_t id[UPLINK_ID_LEN], uint16_t battery_code,
                       uint16_t power_code);

#endif /* UPLINK_H */
