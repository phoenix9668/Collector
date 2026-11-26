/**
 * @file    test_uplink.c
 * @brief   B0 / B2 frame layout (App/protocol/uplink.c).
 */
#include <stdint.h>

#include "check.h"
#include "crc32.h"
#include "sgm58031.h"
#include "uplink.h"

static const uint8_t k_id[4] = {0x0B, 0x31, 0x00, 0x01};

/* A 191-byte collar payload: 187 data bytes + CRC-32 (MSB first), with RFID,
 * stage, battery and reset count where the station decoder expects them. */
static void make_rf_payload(uint8_t *rf, size_t len, uint8_t seed)
{
    size_t body = len - 4u;
    for (size_t i = 0; i < body; i++)
    {
        rf[i] = (uint8_t)(i * 7u + seed);
    }
    if (body >= 187u)
    {
        static const uint8_t rfid[6] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
        memcpy(rf, rfid, sizeof(rfid));
        rf[182] = 5;    /* stage */
        rf[183] = 0x0A; /* collar battery ADC 0x0A28 */
        rf[184] = 0x28;
        rf[185] = 0x00; /* reset count 3 */
        rf[186] = 0x03;
    }
    uint32_t crc = crc32_compute(rf, body);
    rf[body + 0] = (uint8_t)(crc >> 24);
    rf[body + 1] = (uint8_t)(crc >> 16);
    rf[body + 2] = (uint8_t)(crc >> 8);
    rf[body + 3] = (uint8_t)crc;
}

static const collector_time_t k_time = {26, 11, 26, 4, 2, 30, 59};

void test_uplink(void)
{
    uint8_t rf[191];
    uint8_t frame[300];

    make_rf_payload(rf, sizeof(rf), 3);
    CHECK(uplink_rf_crc_ok(rf, sizeof(rf)));
    rf[10] ^= 0x01;
    CHECK(!uplink_rf_crc_ok(rf, sizeof(rf)));
    rf[10] ^= 0x01;
    CHECK(!uplink_rf_crc_ok(rf, 3));

    size_t n = uplink_build_b0(frame, sizeof(frame), k_id, rf, sizeof(rf), 0x9C, &k_time);
    CHECK_EQ(n, 206);
    CHECK_MEM(frame, k_id, 4);
    CHECK_EQ(frame[4], 0xB0);
    CHECK_MEM(&frame[5], rf, 187);
    CHECK_EQ(frame[192], 0x9C);
    const uint8_t time_bytes[7] = {26, 11, 26, 4, 2, 30, 59};
    CHECK_MEM(&frame[193], time_bytes, 7);
    uint32_t crc = crc32_compute(frame, 200);
    CHECK_EQ(frame[200], (crc >> 24) & 0xFF);
    CHECK_EQ(frame[201], (crc >> 16) & 0xFF);
    CHECK_EQ(frame[202], (crc >> 8) & 0xFF);
    CHECK_EQ(frame[203], crc & 0xFF);
    CHECK_EQ(frame[204], 0x0D);
    CHECK_EQ(frame[205], 0x0A);

    /* Length always follows the RF payload: L + 15. */
    CHECK_EQ(uplink_build_b0(frame, sizeof(frame), k_id, rf, 4, 0, &k_time), 19);
    CHECK_EQ(uplink_build_b0(frame, sizeof(frame), k_id, rf, 3, 0, &k_time), 0);
    CHECK_EQ(uplink_build_b0(frame, 205, k_id, rf, sizeof(rf), 0, &k_time), 0);

    n = uplink_build_b2(frame, sizeof(frame), k_id, 7705, 13);
    const uint8_t b2[9] = {0x0B, 0x31, 0x00, 0x01, 0xB2, 0x1E, 0x19, 0x00, 0x0D};
    CHECK_EQ(n, 9);
    CHECK_MEM(frame, b2, 9);
    CHECK_EQ(uplink_build_b2(frame, 8, k_id, 1, 2), 0);
}

void dump_frames(FILE *out)
{
    uint8_t rf[191];
    uint8_t frame[300];

    for (uint8_t seed = 1; seed <= 2; seed++)
    {
        make_rf_payload(rf, sizeof(rf), seed);
        uint8_t rssi = (uint8_t)(seed == 1 ? 0x30 : 0xF0);
        size_t n = uplink_build_b0(frame, sizeof(frame), k_id, rf, sizeof(rf), rssi, &k_time);
        fprintf(out, "B0 ");
        for (size_t i = 0; i < n; i++)
        {
            fprintf(out, "%02X", frame[i]);
        }
        fprintf(out, " rfid=010203040506 stage=5 reset=3 rssi_raw=%u\n", rssi);
    }

    static const int16_t codes[][2] = {{7705, 13}, {9164, 9143}, {-5, 9000}};
    for (size_t k = 0; k < sizeof(codes) / sizeof(codes[0]); k++)
    {
        uint16_t bat = sgm58031_uplink_code(codes[k][0]);
        uint16_t pwr = sgm58031_uplink_code(codes[k][1]);
        size_t n = uplink_build_b2(frame, sizeof(frame), k_id, bat, pwr);
        fprintf(out, "B2 ");
        for (size_t i = 0; i < n; i++)
        {
            fprintf(out, "%02X", frame[i]);
        }
        fprintf(out, " battery_code=%u power_code=%u\n", bat, pwr);
    }
}
