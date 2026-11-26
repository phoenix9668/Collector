/**
 * @file    downlink.c
 * @brief   Downlink frame parsing (formats in downlink.h).
 */
#include "downlink.h"

#include <string.h>

static const char k_heartbeat[] = "Heartbeat";

bool downlink_has_heartbeat(const uint8_t *frame, size_t len)
{
    const size_t hb = sizeof(k_heartbeat) - 1u;
    for (size_t i = 0; i + hb <= len; i++)
    {
        if (memcmp(&frame[i], k_heartbeat, hb) == 0)
        {
            return true;
        }
    }
    return false;
}

static int nibble(uint8_t c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    if (c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }
    if (c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }
    return -1;
}

int downlink_hex_decode(const uint8_t *ascii, size_t len, uint8_t *out, size_t cap)
{
    while (len > 0u && (ascii[len - 1u] == '\r' || ascii[len - 1u] == '\n' || ascii[len - 1u] == ' '))
    {
        len--;
    }
    if (len == 0u || (len % 2u) != 0u || len / 2u > cap)
    {
        return -1;
    }
    for (size_t i = 0; i < len / 2u; i++)
    {
        int hi = nibble(ascii[2u * i]);
        int lo = nibble(ascii[2u * i + 1u]);
        if (hi < 0 || lo < 0)
        {
            return -1;
        }
        out[i] = (uint8_t)((hi << 4) | lo);
    }
    return (int)(len / 2u);
}

static uint8_t byte_or_zero(const uint8_t *b, int n, int i)
{
    return (i < n) ? b[i] : 0u;
}

dl_kind_t downlink_parse(const uint8_t *frame, size_t len, const uint8_t id[4], dl_command_t *cmd)
{
    uint8_t b[DOWNLINK_HEX_MAX];

    memset(cmd, 0, sizeof(*cmd));
    int n = downlink_hex_decode(frame, len, b, sizeof(b));
    if (n < 0)
    {
        return DL_IGNORED;
    }

    if (n >= 2 && b[0] == 0xE5u && b[1] == 0x5Eu)
    {
        cmd->kind = DL_MEMORY;
        cmd->op = byte_or_zero(b, n, 2);
        cmd->address = (uint16_t)((byte_or_zero(b, n, 3) << 8) | byte_or_zero(b, n, 4));
        cmd->data = ((uint32_t)byte_or_zero(b, n, 5) << 24) | ((uint32_t)byte_or_zero(b, n, 6) << 16) |
                    ((uint32_t)byte_or_zero(b, n, 7) << 8) | (uint32_t)byte_or_zero(b, n, 8);
        return DL_MEMORY;
    }

    if (n >= 5 && memcmp(b, id, 4) == 0)
    {
        cmd->kind = DL_FUNCTION;
        cmd->op = b[4];
        if (cmd->op == DL_FN_SET_TIME && n >= 12)
        {
            cmd->has_time = true;
            cmd->time.year = b[5];
            cmd->time.month = b[6];
            cmd->time.date = b[7];
            cmd->time.weekday = b[8];
            cmd->time.hours = b[9];
            cmd->time.minutes = b[10];
            cmd->time.seconds = b[11];
        }
        return DL_FUNCTION;
    }

    return DL_IGNORED;
}
