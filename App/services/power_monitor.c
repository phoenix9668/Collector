/**
 * @file    power_monitor.c
 * @brief   SGM58031 measurement cycle and B2 frames (see power_monitor.h).
 */
#include "power_monitor.h"

#include <stdbool.h>

#include "app_config.h"
#include "app_main.h"
#include "link_uart.h"
#include "log.h"
#include "sensor_i2c.h"
#include "sgm58031.h"
#include "station_status.h"
#include "uplink.h"

#if APP_SGM58031_ENABLE

typedef enum
{
    CHIP_UNKNOWN,
    CHIP_PRESENT,
    CHIP_ABSENT,
} chip_state_t;

static uint32_t s_next_due;
static chip_state_t s_chip = CHIP_UNKNOWN;

static void set_chip_state(chip_state_t state)
{
    if (state != s_chip)
    {
        log_status(state == CHIP_PRESENT ? "SGM58031 ok" : "SGM58031 not found");
        s_chip = state;
    }
}

static void measure_and_send(void)
{
    const sgm58031_bus_t *bus = &sensor_i2c_sgm58031;

    sgm58031_status_t st = sgm58031_probe(bus);
    if (st != SGM58031_OK)
    {
        set_chip_state(CHIP_ABSENT);
        station_status_supply(SUPPLY_CHIP_ABSENT, 0, 0);
        sensor_i2c_recover();
        return;
    }
    set_chip_state(CHIP_PRESENT);

    int16_t battery = 0;
    int16_t power = 0;
    st = sgm58031_measure(bus, SGM58031_MUX_AIN2_AIN3, &battery);
    if (st == SGM58031_OK)
    {
        st = sgm58031_measure(bus, SGM58031_MUX_AIN0_AIN1, &power);
    }
    if (st != SGM58031_OK)
    {
        /* No B2 this period: a 0 would read as a dead battery or lost supply. */
        log_status("SGM58031 read error %u", (unsigned)st);
        station_status_supply(SUPPLY_READ_ERROR, 0, 0);
        sensor_i2c_recover();
        return;
    }

    uint16_t battery_code = sgm58031_uplink_code(battery);
    uint16_t power_code = sgm58031_uplink_code(power);
    uint8_t frame[UPLINK_B2_LEN];
    size_t n = uplink_build_b2(frame, sizeof(frame), app_collector_id(), battery_code, power_code);
    (void)link_uart_write(frame, n);
    station_status_supply(SUPPLY_OK, battery_code, power_code);
    LOG_DEBUG("B2 battery code %d, power code %d", battery, power);
}

void power_monitor_start(uint32_t now)
{
    s_next_due = now + APP_B2_FIRST_DELAY_MS;
}

void power_monitor_poll(uint32_t now)
{
    if ((int32_t)(now - s_next_due) < 0)
    {
        return;
    }
    s_next_due = now + APP_B2_PERIOD_MS;
    measure_and_send();
}

#else /* !APP_SGM58031_ENABLE */

void power_monitor_start(uint32_t now)
{
    (void)now;
}

void power_monitor_poll(uint32_t now)
{
    (void)now;
}

#endif
