/**
 * @file    uplink.c
 * @brief   B0 / B2 frame builders (layout in uplink.h).
 */
#include "uplink.h"

#include <string.h>

#include "crc32.h"

static void put_be32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v >> 24);
    p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >> 8);
    p[3] = (uint8_t)v;
}

static uint32_t get_be32(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

bool uplink_rf_crc_ok(const uint8_t *rf, size_t rf_len)
{
    if (rf_len < UPLINK_RF_CRC_LEN)
    {
        return false;
    }
    size_t body = rf_len - UPLINK_RF_CRC_LEN;
    return crc32_compute(rf, body) == get_be32(&rf[body]);
}

size_t uplink_build_b0(uint8_t *out, size_t cap, const uint8_t id[UPLINK_ID_LEN], const uint8_t *rf,
                       size_t rf_len, uint8_t rssi_raw, const collector_time_t *time)
{
    if (rf_len < UPLINK_RF_CRC_LEN || cap < rf_len + UPLINK_B0_OVERHEAD)
    {
        return 0;
    }
    size_t body = rf_len - UPLINK_RF_CRC_LEN;
    size_t n = 0;

    memcpy(&out[n], id, UPLINK_ID_LEN);
    n += UPLINK_ID_LEN;
    out[n++] = UPLINK_FN_B0;
    memcpy(&out[n], rf, body);
    n += body;
    out[n++] = rssi_raw;
    out[n++] = time->year;
    out[n++] = time->month;
    out[n++] = time->date;
    out[n++] = time->weekday;
    out[n++] = time->hours;
    out[n++] = time->minutes;
    out[n++] = time->seconds;
    put_be32(&out[n], crc32_compute(out, n));
    n += 4u;
    out[n++] = 0x0Du;
    out[n++] = 0x0Au;
    return n;
}

size_t uplink_build_b2(uint8_t *out, size_t cap, const uint8_t id[UPLINK_ID_LEN], uint16_t battery_code,
                       uint16_t power_code)
{
    if (cap < UPLINK_B2_LEN)
    {
        return 0;
    }
    memcpy(out, id, UPLINK_ID_LEN);
    out[4] = UPLINK_FN_B2;
    out[5] = (uint8_t)(battery_code >> 8);
    out[6] = (uint8_t)battery_code;
    out[7] = (uint8_t)(power_code >> 8);
    out[8] = (uint8_t)power_code;
    return UPLINK_B2_LEN;
}
