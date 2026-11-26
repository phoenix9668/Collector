/**
 * @file    sensor_i2c.h
 * @brief   I2C1 (PB6 SCL / PB7 SDA, 100 kHz) to the power management board,
 *          bound to the SGM58031 driver's bus interface.
 *
 * The bus only reaches the SGM58031 when the jumper on the power board routes
 * its I2C to the RFID board. Without it every transfer fails; callers report
 * that and carry on.
 */
#ifndef SENSOR_I2C_H
#define SENSOR_I2C_H

#include "sgm58031.h"

/* Bus interface for sgm58031_probe() / sgm58031_measure(). Task context. */
extern const sgm58031_bus_t sensor_i2c_sgm58031;

/* Frees a stuck bus (9 SCL clocks + STOP) and re-initialises I2C1. */
void sensor_i2c_recover(void);

#endif /* SENSOR_I2C_H */
