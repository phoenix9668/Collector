/**
 * @file    test_status.c
 * @brief   Status lamp: fault evaluation and blink sequences
 *          (App/services/status_logic.c).
 */
#include <stdint.h>

#include "app_config.h"
#include "check.h"
#include "status_logic.h"

static const station_limits_t k_limits = {
    .uplink_timeout_ms = APP_STATUS_UPLINK_TIMEOUT_MS,
    .collar_timeout_ms = APP_STATUS_COLLAR_TIMEOUT_MS,
    .supply_min_mv = APP_STATUS_SUPPLY_MIN_MV,
    .battery_low_mv = APP_STATUS_BATTERY_LOW_MV,
};

/* A healthy station 1 h after boot: everything recent, on mains. */
static station_snapshot_t healthy(void)
{
    station_snapshot_t s;
    memset(&s, 0, sizeof(s));
    s.now_ms = 3600000u;
    s.radio_started = true;
    s.radio_ok = true;
    s.collar_seen = true;
    s.last_collar_ms = s.now_ms - 60000u;
    s.heartbeat_seen = true;
    s.last_heartbeat_ms = s.now_ms - 60000u;
    s.supply_monitored = true;
    s.supply = SUPPLY_OK;
    s.battery_code = 8802; /* 12.10 V */
    s.power_code = 9142;   /* 24.0 V */
    return s;
}

/* Runs the engine for n steps and returns "1"/"0" per 100 ms step. */
static const char *run(led_engine_t *e, int n, bool booting, uint32_t faults, volatile bool *activity)
{
    static char out[128];
    for (int i = 0; i < n && i < (int)sizeof(out) - 1; i++)
    {
        out[i] = led_engine_step(e, booting, faults, activity) ? '1' : '0';
        out[i + 1] = '\0';
    }
    return out;
}

static void test_faults(void)
{
    station_snapshot_t s;

    CHECK_EQ(battery_mv(8802), 12102); /* bench reading: 12.103 V */
    CHECK_EQ(supply_mv(31), 81);       /* bench reading: 0.081 V */
    CHECK_EQ(supply_mv(9142), 23997);

    s = healthy();
    CHECK_EQ(station_faults(&s, &k_limits), 0);

    /* 1: uplink. 6 min without Heartbeat; before any, counted from boot. */
    s = healthy();
    s.last_heartbeat_ms = s.now_ms - APP_STATUS_UPLINK_TIMEOUT_MS;
    CHECK_EQ(station_faults(&s, &k_limits), 0);
    s.last_heartbeat_ms -= 1;
    CHECK_EQ(station_faults(&s, &k_limits), STATION_FAULT_UPLINK);
    s = healthy();
    s.heartbeat_seen = false;
    s.now_ms = APP_STATUS_UPLINK_TIMEOUT_MS;
    s.last_collar_ms = 0;
    CHECK_EQ(station_faults(&s, &k_limits) & STATION_FAULT_UPLINK, 0);
    s.now_ms += 1;
    CHECK(station_faults(&s, &k_limits) & STATION_FAULT_UPLINK);

    /* 2: collars, 30 min. */
    s = healthy();
    s.last_collar_ms = s.now_ms - APP_STATUS_COLLAR_TIMEOUT_MS - 1u;
    CHECK_EQ(station_faults(&s, &k_limits), STATION_FAULT_NO_COLLAR);
    s = healthy();
    s.collar_seen = false;
    s.now_ms = 25u * 60u * 1000u;
    s.last_heartbeat_ms = s.now_ms;
    CHECK_EQ(station_faults(&s, &k_limits), 0);

    /* Tick wrap: last event just before 2^32, now just after. */
    s = healthy();
    s.now_ms = 1000u;
    s.last_collar_ms = 0xFFFFF000u;
    s.last_heartbeat_ms = 0xFFFFF000u;
    CHECK_EQ(station_faults(&s, &k_limits), 0);

    /* 3 / 4: supply and battery from the last B2. */
    s = healthy();
    s.power_code = 31; /* the bench without a power adapter */
    CHECK_EQ(station_faults(&s, &k_limits), STATION_FAULT_ON_BATTERY);
    s = healthy();
    s.power_code = (uint16_t)(APP_STATUS_SUPPLY_MIN_MV * 8u / 21u + 1u);
    CHECK_EQ(station_faults(&s, &k_limits), 0);
    s = healthy();
    s.battery_code = 7563; /* 10.39 V */
    CHECK_EQ(station_faults(&s, &k_limits), STATION_FAULT_BATTERY_LOW);
    s.power_code = 0;
    CHECK_EQ(station_faults(&s, &k_limits), STATION_FAULT_BATTERY_LOW | STATION_FAULT_ON_BATTERY);

    /* 5: SGM58031 missing or failing; nothing before the first cycle or when
     * the feature is off. */
    s = healthy();
    s.supply = SUPPLY_CHIP_ABSENT;
    s.power_code = 0;
    CHECK_EQ(station_faults(&s, &k_limits), STATION_FAULT_SUPPLY_SENSOR);
    s.supply = SUPPLY_READ_ERROR;
    CHECK_EQ(station_faults(&s, &k_limits), STATION_FAULT_SUPPLY_SENSOR);
    s.supply = SUPPLY_UNKNOWN;
    CHECK_EQ(station_faults(&s, &k_limits), 0);
    s.supply = SUPPLY_CHIP_ABSENT;
    s.supply_monitored = false;
    CHECK_EQ(station_faults(&s, &k_limits), 0);

    /* 6: radio, only once it has been started. */
    s = healthy();
    s.radio_ok = false;
    CHECK_EQ(station_faults(&s, &k_limits), STATION_FAULT_RADIO);
    s.radio_started = false;
    CHECK_EQ(station_faults(&s, &k_limits), 0);
}

static void test_engine(void)
{
    led_engine_t e;
    volatile bool activity = false;

    /* Booting: 5 Hz. */
    led_engine_init(&e);
    CHECK_STR(run(&e, 8, true, 0, &activity), "10101010");

    /* Normal: 100 ms on every 3 s. */
    led_engine_init(&e);
    CHECK_STR(run(&e, 60, false, 0, &activity),
              "100000000000000000000000000000"
              "100000000000000000000000000000");

    /* A collar packet: the next normal cycle flashes twice, then back to one. */
    led_engine_init(&e);
    activity = true;
    CHECK_STR(run(&e, 60, false, 0, &activity),
              "101000000000000000000000000000"
              "100000000000000000000000000000");
    CHECK(!activity);

    /* Fault code 2: two 200 ms flashes 300 ms apart, then 2 s dark. */
    led_engine_init(&e);
    CHECK_STR(run(&e, 60, false, STATION_FAULT_NO_COLLAR, &activity),
              "110001100000000000000000000000"
              "110001100000000000000000000000");

    /* Codes 1 and 3 take turns; activity is not shown while faults are. */
    led_engine_init(&e);
    activity = true;
    CHECK_STR(run(&e, 85, false, STATION_FAULT_UPLINK | STATION_FAULT_ON_BATTERY, &activity),
              "1100000000000000000000000"           /* code 1: 25 steps */
              "11000110001100000000000000000000000" /* code 3: 35 steps */
              "1100000000000000000000000");         /* code 1 again */
    CHECK(activity);

    /* Code 6 fits: 6 flashes + gap = 50 steps. */
    led_engine_init(&e);
    CHECK_STR(run(&e, 50, false, STATION_FAULT_RADIO, &activity),
              "11000110001100011000110001100000000000000000000000");

    /* Faults clear: the running cycle finishes, then normal resumes. */
    led_engine_init(&e);
    (void)run(&e, 10, false, STATION_FAULT_UPLINK, &activity);
    activity = false;
    CHECK_STR(run(&e, 46, false, 0, &activity),
              "000000000000000"                 /* rest of code 1's cycle */
              "100000000000000000000000000000"  /* normal, 30 steps */
              "1");                             /* next normal cycle */
}

void test_status(void)
{
    test_faults();
    test_engine();
}
