/**
 * @file    status_logic.h
 * @brief   Station health for the P4 status lamp: which faults are active and
 *          how the lamp blinks. Pure functions (no HAL, no RTOS), host-tested;
 *          App/services/station_status.c feeds them on the target.
 *
 * Lamp language, 100 ms steps:
 *   booting (radio not started yet)  5 Hz: on 100 ms, off 100 ms
 *   normal                           on 100 ms every 3 s
 *   normal + a collar packet arrived that cycle flashes twice instead
 *   fault code N                     N x (on 200, off 300), then off 2 s;
 *                                    several faults take turns, lowest code first
 */
#ifndef STATUS_LOGIC_H
#define STATUS_LOGIC_H

#include <stdbool.h>
#include <stdint.h>

/* Fault bits; fault code = bit index + 1. */
#define STATION_FAULT_UPLINK         (1u << 0) /* 1: no Heartbeat (ESP32_V2 / station / 4G) */
#define STATION_FAULT_NO_COLLAR      (1u << 1) /* 2: no valid collar packet */
#define STATION_FAULT_ON_BATTERY     (1u << 2) /* 3: external supply missing */
#define STATION_FAULT_BATTERY_LOW    (1u << 3) /* 4: battery voltage low */
#define STATION_FAULT_SUPPLY_SENSOR  (1u << 4) /* 5: SGM58031 missing or read error */
#define STATION_FAULT_RADIO          (1u << 5) /* 6: CC1101 init failed */
#define STATION_FAULT_COUNT          6u

typedef enum
{
    SUPPLY_UNKNOWN = 0,  /* no SGM58031 cycle yet */
    SUPPLY_OK,           /* last cycle read both channels */
    SUPPLY_CHIP_ABSENT,
    SUPPLY_READ_ERROR,
} supply_state_t;

typedef struct
{
    uint32_t now_ms;            /* uptime */
    bool radio_started;         /* first CC1101 bring-up done */
    bool radio_ok;              /* result of the last bring-up */
    bool collar_seen;
    uint32_t last_collar_ms;    /* last valid collar packet */
    bool heartbeat_seen;
    uint32_t last_heartbeat_ms; /* last "Heartbeat" from the uplink */
    bool supply_monitored;      /* APP_SGM58031_ENABLE */
    supply_state_t supply;
    uint16_t battery_code;      /* last B2 codes */
    uint16_t power_code;
} station_snapshot_t;

typedef struct
{
    uint32_t uplink_timeout_ms;
    uint32_t collar_timeout_ms;
    uint32_t supply_min_mv;
    uint32_t battery_low_mv;
} station_limits_t;

/* SGM58031 code (+/-4.096 V, 1/8 mV per LSB) to millivolts at the divider input. */
static inline uint32_t battery_mv(uint16_t code) { return (uint32_t)code * 11u / 8u; }
static inline uint32_t supply_mv(uint16_t code) { return (uint32_t)code * 21u / 8u; }

uint32_t station_faults(const station_snapshot_t *s, const station_limits_t *lim);

typedef struct
{
    uint64_t bits;  /* step i is on when bit i is set */
    uint8_t len;    /* steps in the current cycle */
    uint8_t pos;    /* next step */
    uint8_t last_code;
} led_engine_t;

void led_engine_init(led_engine_t *e);

/* Advances one 100 ms step and returns whether the lamp is on. A new cycle
 * starts when the previous one ends, using the inputs given at that moment.
 * *activity (a collar packet since the last normal cycle) is cleared when a
 * double flash consumes it. */
bool led_engine_step(led_engine_t *e, bool booting, uint32_t faults, volatile bool *activity);

#endif /* STATUS_LOGIC_H */
