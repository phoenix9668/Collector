/**
 * @file    test_downlink.c
 * @brief   Heartbeat detection and command parsing (App/protocol/downlink.c).
 */
#include <stdint.h>

#include "check.h"
#include "downlink.h"

static const uint8_t k_id[4] = {0x0B, 0x31, 0x00, 0x01};

static dl_kind_t parse(const char *text, dl_command_t *cmd)
{
    return downlink_parse((const uint8_t *)text, strlen(text), k_id, cmd);
}

static int has_hb(const char *text)
{
    return downlink_has_heartbeat((const uint8_t *)text, strlen(text));
}

void test_downlink(void)
{
    dl_command_t cmd;
    uint8_t out[8];

    /* Heartbeat: exact, with line endings, merged with another frame. */
    CHECK(has_hb("Heartbeat"));
    CHECK(has_hb("Heartbeat\r\n"));
    CHECK(has_hb("0B310001A9Heartbeat"));
    CHECK(!has_hb("Heartbea"));
    CHECK(!has_hb("heartbeat"));
    CHECK_EQ(parse("Heartbeat", &cmd), DL_IGNORED);

    /* Hex decoding. */
    CHECK_EQ(downlink_hex_decode((const uint8_t *)"0aFF\r\n", 6, out, sizeof(out)), 2);
    CHECK_EQ(out[0], 0x0A);
    CHECK_EQ(out[1], 0xFF);
    CHECK_EQ(downlink_hex_decode((const uint8_t *)"ABC", 3, out, sizeof(out)), -1);
    CHECK_EQ(downlink_hex_decode((const uint8_t *)"AG", 2, out, sizeof(out)), -1);
    CHECK_EQ(downlink_hex_decode((const uint8_t *)"", 0, out, sizeof(out)), -1);
    CHECK_EQ(downlink_hex_decode((const uint8_t *)"000000000000000000", 18, out, sizeof(out)), -1);

    /* E5 5E data EEPROM commands. */
    CHECK_EQ(parse("E55E0100100B310002", &cmd), DL_MEMORY);
    CHECK_EQ(cmd.op, DL_MEM_WRITE);
    CHECK_EQ(cmd.address, 0x0010);
    CHECK_EQ(cmd.data, 0x0B310002u);
    CHECK_EQ(parse("e55e020000", &cmd), DL_MEMORY); /* short: data counts as 0 */
    CHECK_EQ(cmd.op, DL_MEM_READ);
    CHECK_EQ(cmd.address, 0x0000);
    CHECK_EQ(cmd.data, 0u);
    CHECK_EQ(parse("E55E", &cmd), DL_MEMORY);
    CHECK_EQ(cmd.op, 0);

    /* Function calls for this CollectorID. */
    CHECK_EQ(parse("0B310001A0", &cmd), DL_FUNCTION);
    CHECK_EQ(cmd.op, DL_FN_RESET);
    CHECK_EQ(parse("0B310001A81A0B1A04021E3B\r\n", &cmd), DL_FUNCTION);
    CHECK_EQ(cmd.op, DL_FN_SET_TIME);
    CHECK(cmd.has_time);
    CHECK_EQ(cmd.time.year, 26);
    CHECK_EQ(cmd.time.month, 11);
    CHECK_EQ(cmd.time.date, 26);
    CHECK_EQ(cmd.time.weekday, 4);
    CHECK_EQ(cmd.time.hours, 2);
    CHECK_EQ(cmd.time.minutes, 30);
    CHECK_EQ(cmd.time.seconds, 59);
    CHECK(collector_time_valid(&cmd.time));
    CHECK_EQ(parse("0B310001A81A0B", &cmd), DL_FUNCTION); /* too short for a time */
    CHECK(!cmd.has_time);
    CHECK_EQ(parse("0B310001A9", &cmd), DL_FUNCTION);
    CHECK_EQ(cmd.op, DL_FN_READ_TIME);

    /* Not for us / not a command. */
    CHECK_EQ(parse("0B310002A0", &cmd), DL_IGNORED);
    CHECK_EQ(parse("0B3100", &cmd), DL_IGNORED);
    CHECK_EQ(parse("##Read Memory Complete##", &cmd), DL_IGNORED);
}
