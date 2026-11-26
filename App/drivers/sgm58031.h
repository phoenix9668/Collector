/**
 * @file    sgm58031.h
 * @brief   SGM58031 16-bit ADC on the power management board.
 *
 * Port of the 4G firmware's reference driver (Collector-QuecPython,
 * branch feature/rs485-local, src/collector_sensors.py, release 4.1.6):
 *  - address 0x48 (ADDR to GND); CHIP_ID register reads 0x0080;
 *  - each channel is one single-shot conversion: wait until idle, write
 *    CONFIG1 = BUS_FLEX (VDD 5 V, I2C pull-ups 3.3 V), write CONFIG = start |
 *    MUX, wait 2 ms, poll OS (1 ms steps, at most 50), check MUX, read;
 *  - +/-4.096 V, 800 SPS, comparator off.
 * Wiring: AIN2-AIN3 = battery (BATs_Li+ through 1M/100k, x11),
 *         AIN0-AIN1 = supply  (VCC_IN through 2M/100k, x21).
 *
 * The driver is bus-agnostic: App/bsp/sensor_i2c.c binds it to I2C1 and the
 * host tests bind it to a register model.
 */
#ifndef SGM58031_H
#define SGM58031_H

#include <stdint.h>

#define SGM58031_I2C_ADDR          0x48u

#define SGM58031_REG_CONVERSION    0x00u
#define SGM58031_REG_CONFIG        0x01u
#define SGM58031_REG_CONFIG1       0x04u
#define SGM58031_REG_CHIP_ID       0x05u

#define SGM58031_CHIP_ID           0x0080u
#define SGM58031_CONFIG_OS         0x8000u
#define SGM58031_CONFIG_MUX_SHIFT  12u
/* OS (bit 15) | PGA[11:9] = 001, +/-4.096 V | MODE (bit 8) single-shot |
 * DR[7:5] = 111 | COMP_QUE[1:0] = 11, comparator off. MUX[14:12] is added
 * per channel: 0xB3E3 for the battery, 0x83E3 for the supply. */
#define SGM58031_CONFIG_START      (SGM58031_CONFIG_OS | (1u << 9) | (1u << 8) | (7u << 5) | 3u)
#define SGM58031_CONFIG1_BUS_FLEX  0x0010u

#define SGM58031_MUX_AIN0_AIN1     0u /* supply  */
#define SGM58031_MUX_AIN2_AIN3     3u /* battery */

#define SGM58031_SETTLE_MS         2u
#define SGM58031_POLL_LIMIT        50u

typedef enum
{
    SGM58031_OK = 0,
    SGM58031_ERR_BUS,     /* an I2C transfer failed (no chip / no jumper) */
    SGM58031_ERR_ID,      /* answered, but CHIP_ID is not 0x0080 */
    SGM58031_ERR_TIMEOUT, /* OS never reported a finished conversion */
    SGM58031_ERR_MUX,     /* the finished conversion was of another input */
} sgm58031_status_t;

typedef struct
{
    int (*write)(void *ctx, uint8_t reg, uint16_t value); /* 0 on success */
    int (*read)(void *ctx, uint8_t reg, uint16_t *value); /* 0 on success */
    void (*delay_ms)(void *ctx, uint32_t ms);
    void *ctx;
} sgm58031_bus_t;

/* CONFIG word that starts a single-shot conversion of the given MUX. */
uint16_t sgm58031_config_word(uint8_t mux);

/* Value for a B2 frame: negative codes (offset noise at 0 V) become 0,
 * because the station decodes the field as unsigned. */
uint16_t sgm58031_uplink_code(int16_t code);

sgm58031_status_t sgm58031_probe(const sgm58031_bus_t *bus);
sgm58031_status_t sgm58031_measure(const sgm58031_bus_t *bus, uint8_t mux, int16_t *code);

#endif /* SGM58031_H */
