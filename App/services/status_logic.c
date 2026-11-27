/**
 * @file    status_logic.c
 * @brief   Station faults and the status lamp blink engine (see status_logic.h).
 */
#include "status_logic.h"

#include <stddef.h>

/* Steps are 100 ms. */
#define NORMAL_STEPS   30u /* 3 s */
#define CODE_ON_STEPS  2u  /* 200 ms */
#define CODE_OFF_STEPS 3u  /* 300 ms */
#define CODE_GAP_STEPS 20u /* 2 s */

static bool expired(bool seen, uint32_t last_ms, uint32_t now_ms, uint32_t timeout_ms)
{
    /* Never seen: count from boot. Unsigned difference handles tick wrap. */
    uint32_t since = seen ? now_ms - last_ms : now_ms;
    return since > timeout_ms;
}

uint32_t station_faults(const station_snapshot_t *s, const station_limits_t *lim)
{
    uint32_t f = 0;

    if (expired(s->heartbeat_seen, s->last_heartbeat_ms, s->now_ms, lim->uplink_timeout_ms))
    {
        f |= STATION_FAULT_UPLINK;
    }
    if (expired(s->collar_seen, s->last_collar_ms, s->now_ms, lim->collar_timeout_ms))
    {
        f |= STATION_FAULT_NO_COLLAR;
    }
    if (s->supply_monitored)
    {
        if (s->supply == SUPPLY_CHIP_ABSENT || s->supply == SUPPLY_READ_ERROR)
        {
            f |= STATION_FAULT_SUPPLY_SENSOR;
        }
        else if (s->supply == SUPPLY_OK)
        {
            if (supply_mv(s->power_code) < lim->supply_min_mv)
            {
                f |= STATION_FAULT_ON_BATTERY;
            }
            if (battery_mv(s->battery_code) < lim->battery_low_mv)
            {
                f |= STATION_FAULT_BATTERY_LOW;
            }
        }
    }
    if (s->radio_started && !s->radio_ok)
    {
        f |= STATION_FAULT_RADIO;
    }
    return f;
}

void led_engine_init(led_engine_t *e)
{
    e->bits = 0;
    e->len = 0;
    e->pos = 0;
    e->last_code = 0;
}

static void set_on(led_engine_t *e, uint8_t from, uint8_t count)
{
    for (uint8_t i = 0; i < count; i++)
    {
        e->bits |= (uint64_t)1u << (from + i);
    }
}

/* Lowest active fault code above the last one shown, wrapping around. */
static uint8_t next_code(uint32_t faults, uint8_t last)
{
    for (uint8_t i = 1; i <= STATION_FAULT_COUNT; i++)
    {
        uint8_t code = (uint8_t)(((last + i - 1u) % STATION_FAULT_COUNT) + 1u);
        if (faults & (1u << (code - 1u)))
        {
            return code;
        }
    }
    return 0;
}

static void start_cycle(led_engine_t *e, bool booting, uint32_t faults, volatile bool *activity)
{
    e->bits = 0;
    e->pos = 0;

    if (booting)
    {
        e->len = 2;
        set_on(e, 0, 1);
        return;
    }
    uint8_t code = next_code(faults, e->last_code);
    if (code != 0u)
    {
        e->last_code = code;
        uint8_t step = 0;
        for (uint8_t n = 0; n < code; n++)
        {
            set_on(e, step, CODE_ON_STEPS);
            step = (uint8_t)(step + CODE_ON_STEPS + CODE_OFF_STEPS);
        }
        e->len = (uint8_t)(step + CODE_GAP_STEPS);
        return;
    }
    e->last_code = 0;
    e->len = NORMAL_STEPS;
    if (activity != NULL && *activity)
    {
        *activity = false;
        set_on(e, 0, 1); /* on, off, on, off, then dark */
        set_on(e, 2, 1);
    }
    else
    {
        set_on(e, 0, 1);
    }
}

bool led_engine_step(led_engine_t *e, bool booting, uint32_t faults, volatile bool *activity)
{
    if (e->pos >= e->len)
    {
        start_cycle(e, booting, faults, activity);
    }
    bool on = (e->bits >> e->pos) & 1u;
    e->pos++;
    return on;
}
