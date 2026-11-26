/**
 * @file    downlink.h
 * @brief   Frames the 4G module / ESP32 adapter sends on USART1.
 *
 * A frame is whatever arrived before the line went idle:
 *  - "Heartbeat" (9 ASCII bytes): keeps the module's watchdog in
 *    modem_supervisor from resetting it. Found anywhere in the frame.
 *  - ASCII hex (even length; trailing CR/LF/space ignored), decoded to:
 *      E5 5E op addrHi addrLo d3 d2 d1 d0  data EEPROM access
 *          op 01 write word, 02 read word, 03 write 0
 *          missing trailing bytes count as 0 (as the firmware always did)
 *      ID3 ID2 ID1 ID0 fn [args]           function call for this CollectorID
 *          A0 reset
 *          A8 YY MM DD WD hh mm ss         set the RTC
 *          A9 read the RTC
 *  - anything else is ignored.
 */
#ifndef DOWNLINK_H
#define DOWNLINK_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "collector_time.h"

#define DOWNLINK_HEX_MAX 32u /* decoded bytes */

#define DL_FN_RESET     0xA0u
#define DL_FN_SET_TIME  0xA8u
#define DL_FN_READ_TIME 0xA9u

#define DL_MEM_WRITE 0x01u
#define DL_MEM_READ  0x02u
#define DL_MEM_ERASE 0x03u

typedef enum
{
    DL_IGNORED = 0,
    DL_MEMORY,
    DL_FUNCTION,
} dl_kind_t;

typedef struct
{
    dl_kind_t kind;
    uint8_t op;       /* DL_MEM_xxx for DL_MEMORY, DL_FN_xxx for DL_FUNCTION */
    uint16_t address; /* DL_MEMORY: offset into the data EEPROM */
    uint32_t data;    /* DL_MEMORY: word to write */
    bool has_time;    /* DL_FN_SET_TIME with all 7 time bytes present */
    collector_time_t time;
} dl_command_t;

bool downlink_has_heartbeat(const uint8_t *frame, size_t len);

/* Decodes ASCII hex. Returns the byte count, or -1 for an odd length, a
 * non-hex character or more than cap bytes. */
int downlink_hex_decode(const uint8_t *ascii, size_t len, uint8_t *out, size_t cap);

dl_kind_t downlink_parse(const uint8_t *frame, size_t len, const uint8_t id[4], dl_command_t *cmd);

#endif /* DOWNLINK_H */
