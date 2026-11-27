/**
 * @file    station_status.c
 * @brief   Collects station health and drives the P4 status lamp
 *          (see station_status.h).
 *
 * Every field below has a single writer task and is a 32-bit (or smaller)
 * aligned variable, so readers never see a torn value.
 */
#include "station_status.h"

#include "app_config.h"
#include "board.h"
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "modem_supervisor.h"
#include "timers.h"

#define LED_TICK_MS 100u

/* Radio task */
static volatile bool s_radio_started;
static volatile bool s_radio_ok;
static volatile bool s_collar_seen;
static volatile uint32_t s_last_collar_ms;
static volatile bool s_activity; /* cleared by the lamp timer */

/* Housekeeping task */
static volatile supply_state_t s_supply = SUPPLY_UNKNOWN;
static volatile uint16_t s_battery_code;
static volatile uint16_t s_power_code;
static volatile uint32_t s_faults;

/* Timer task */
static led_engine_t s_engine;
static StaticTimer_t s_timer_buffer;
static TimerHandle_t s_timer;

static const station_limits_t k_limits = {
    .uplink_timeout_ms = APP_STATUS_UPLINK_TIMEOUT_MS,
    .collar_timeout_ms = APP_STATUS_COLLAR_TIMEOUT_MS,
    .supply_min_mv = APP_STATUS_SUPPLY_MIN_MV,
    .battery_low_mv = APP_STATUS_BATTERY_LOW_MV,
};

static void lamp_timer(TimerHandle_t timer)
{
    (void)timer;
    /* Booting until the radio has started: the first 20 s after reset. */
    board_status_led(led_engine_step(&s_engine, !s_radio_started, s_faults, &s_activity));
}

void station_status_start(void)
{
    led_engine_init(&s_engine);
    /* Native FreeRTOS: osTimerNew() would malloc its callback wrapper. */
    s_timer = xTimerCreateStatic("statusLed", pdMS_TO_TICKS(LED_TICK_MS), pdTRUE, NULL, lamp_timer,
                                 &s_timer_buffer);
    if (s_timer != NULL)
    {
        (void)xTimerStart(s_timer, 0);
    }
}

void station_status_tick(uint32_t now_ms)
{
    station_snapshot_t s;
    uint32_t last_heartbeat = 0;

    s.now_ms = now_ms;
    s.radio_started = s_radio_started;
    s.radio_ok = s_radio_ok;
    s.collar_seen = s_collar_seen;
    s.last_collar_ms = s_last_collar_ms;
    s.heartbeat_seen = modem_supervisor_last_heartbeat(&last_heartbeat);
    s.last_heartbeat_ms = last_heartbeat;
    s.supply_monitored = APP_SGM58031_ENABLE != 0;
    s.supply = s_supply;
    s.battery_code = s_battery_code;
    s.power_code = s_power_code;
    s_faults = station_faults(&s, &k_limits);
}

void station_status_radio(bool ok)
{
    s_radio_ok = ok;
    s_radio_started = true;
}

void station_status_collar_ok(void)
{
    s_last_collar_ms = osKernelGetTickCount();
    s_collar_seen = true;
    s_activity = true;
}

void station_status_supply(supply_state_t state, uint16_t battery_code, uint16_t power_code)
{
    s_battery_code = battery_code;
    s_power_code = power_code;
    s_supply = state;
}
