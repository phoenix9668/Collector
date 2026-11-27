/**
 * @file    station_status.h
 * @brief   Station health as shown on the P4 status lamp (PA8, STATUS_LED).
 *
 * The services report events here; the housekeeping task turns them into
 * fault bits once a second (status_logic.c) and a 100 ms FreeRTOS software
 * timer plays the matching blink pattern, so the lamp keeps going while
 * housekeeping is busy with a PWRKEY pulse or an SGM58031 measurement.
 * Codes and thresholds: README "状态灯（P4）", app_config.h APP_STATUS_*.
 */
#ifndef STATION_STATUS_H
#define STATION_STATUS_H

#include <stdbool.h>
#include <stdint.h>

#include "status_logic.h"

/* Housekeeping task, once at start: creates and starts the lamp timer. */
void station_status_start(void);

/* Housekeeping task, once a second: re-evaluates the faults. */
void station_status_tick(uint32_t now_ms);

/* Radio task: result of each CC1101 bring-up. */
void station_status_radio(bool ok);

/* Radio task: a valid collar packet went out as B0 (also flashes the lamp). */
void station_status_collar_ok(void);

/* Housekeeping task (power monitor): outcome of each SGM58031 cycle. */
void station_status_supply(supply_state_t state, uint16_t battery_code, uint16_t power_code);

#endif /* STATION_STATUS_H */
