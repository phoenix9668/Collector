/**
 * @file    power_monitor.h
 * @brief   Battery and supply voltages from the SGM58031, sent as B2 frames.
 *
 * First frame APP_B2_FIRST_DELAY_MS after boot, then every APP_B2_PERIOD_MS.
 * Without the SGM58031 (I2C jumper not fitted) the board reports
 * "##SGM58031 not found##" once, sends no B2 and keeps running; it looks for
 * the chip again at every period. Compiled out with APP_SGM58031_ENABLE = 0.
 */
#ifndef POWER_MONITOR_H
#define POWER_MONITOR_H

#include <stdint.h>

void power_monitor_start(uint32_t now);

/* Housekeeping task, once a second. A measurement blocks for ~10-120 ms. */
void power_monitor_poll(uint32_t now);

#endif /* POWER_MONITOR_H */
