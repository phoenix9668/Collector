/**
 * @file    modem_supervisor.h
 * @brief   Keeps the 4G module (EC600X) alive.
 *
 * - PWRKEY (PB3) pulsed low for 2 s at boot and every 2 h, as the firmware
 *   always did.
 * - The module (or the ESP32 adapter) sends "Heartbeat" on USART1. Without
 *   one for 10 minutes RESET (PB8) is pulled low for 500 ms.
 * Driven by the housekeeping task; the link task reports heartbeats.
 */
#ifndef MODEM_SUPERVISOR_H
#define MODEM_SUPERVISOR_H

#include <stdbool.h>
#include <stdint.h>

/* Boot pulse on PWRKEY; blocks for APP_MODEM_PWRKEY_PULSE_MS. */
void modem_supervisor_start(void);

/* Any task: a "Heartbeat" arrived. */
void modem_supervisor_on_heartbeat(void);

/* Tick of the last real "Heartbeat" (reset pulses don't count); false if
 * none has arrived since boot. */
bool modem_supervisor_last_heartbeat(uint32_t *tick_ms);

/* Housekeeping task, once a second. May block for a pulse. */
void modem_supervisor_poll(uint32_t now);

#endif /* MODEM_SUPERVISOR_H */
