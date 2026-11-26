/**
 * @file    sensor_i2c.c
 * @brief   I2C1 binding for the SGM58031 (see sensor_i2c.h).
 *
 * Register access is a pointer write followed by a separate read, with a
 * STOP in between, as the original driver and the 4G firmware's reference
 * driver both do.
 */
#include "sensor_i2c.h"

#include "cmsis_os2.h"
#include "i2c.h"

#define SENSOR_I2C_ADDR8      ((uint16_t)(SGM58031_I2C_ADDR << 1))
#define SENSOR_I2C_TIMEOUT_MS 10u

#define SCL_PIN LL_GPIO_PIN_6
#define SDA_PIN LL_GPIO_PIN_7

static int bus_write(void *ctx, uint8_t reg, uint16_t value)
{
    (void)ctx;
    uint8_t buf[3] = {reg, (uint8_t)(value >> 8), (uint8_t)value};
    return HAL_I2C_Master_Transmit(&hi2c1, SENSOR_I2C_ADDR8, buf, sizeof(buf), SENSOR_I2C_TIMEOUT_MS) == HAL_OK ? 0 : -1;
}

static int bus_read(void *ctx, uint8_t reg, uint16_t *value)
{
    (void)ctx;
    uint8_t buf[2];
    if (HAL_I2C_Master_Transmit(&hi2c1, SENSOR_I2C_ADDR8, &reg, 1, SENSOR_I2C_TIMEOUT_MS) != HAL_OK)
    {
        return -1;
    }
    if (HAL_I2C_Master_Receive(&hi2c1, SENSOR_I2C_ADDR8, buf, sizeof(buf), SENSOR_I2C_TIMEOUT_MS) != HAL_OK)
    {
        return -1;
    }
    *value = (uint16_t)((buf[0] << 8) | buf[1]);
    return 0;
}

static void bus_delay_ms(void *ctx, uint32_t ms)
{
    (void)ctx;
    osDelay(ms);
}

const sgm58031_bus_t sensor_i2c_sgm58031 = {
    .write = bus_write,
    .read = bus_read,
    .delay_ms = bus_delay_ms,
    .ctx = NULL,
};

static void half_bit_delay(void)
{
    /* ~5 us at 32 MHz: 100 kHz bus timing for the recovery clocks. */
    for (volatile uint32_t i = 0; i < 40u; i++)
    {
    }
}

void sensor_i2c_recover(void)
{
    (void)HAL_I2C_DeInit(&hi2c1);

    LL_GPIO_InitTypeDef gpio = {0};
    gpio.Pin = SCL_PIN | SDA_PIN;
    gpio.Mode = LL_GPIO_MODE_OUTPUT;
    gpio.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    gpio.Pull = LL_GPIO_PULL_UP;
    gpio.Speed = LL_GPIO_SPEED_FREQ_LOW;
    LL_GPIO_SetOutputPin(GPIOB, SCL_PIN | SDA_PIN);
    LL_GPIO_Init(GPIOB, &gpio);
    half_bit_delay();

    /* Clock out whatever byte a slave may still be driving. */
    for (int i = 0; i < 9 && !LL_GPIO_IsInputPinSet(GPIOB, SDA_PIN); i++)
    {
        LL_GPIO_ResetOutputPin(GPIOB, SCL_PIN);
        half_bit_delay();
        LL_GPIO_SetOutputPin(GPIOB, SCL_PIN);
        half_bit_delay();
    }
    /* STOP: SDA rises while SCL is high. */
    LL_GPIO_ResetOutputPin(GPIOB, SCL_PIN);
    half_bit_delay();
    LL_GPIO_ResetOutputPin(GPIOB, SDA_PIN);
    half_bit_delay();
    LL_GPIO_SetOutputPin(GPIOB, SCL_PIN);
    half_bit_delay();
    LL_GPIO_SetOutputPin(GPIOB, SDA_PIN);
    half_bit_delay();

    /* HAL_I2C_MspInit() puts PB6/PB7 back into I2C mode. */
    (void)HAL_I2C_Init(&hi2c1);
}
