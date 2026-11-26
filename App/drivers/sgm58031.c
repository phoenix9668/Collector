/**
 * @file    sgm58031.c
 * @brief   SGM58031 single-shot measurement (see sgm58031.h).
 */
#include "sgm58031.h"

uint16_t sgm58031_config_word(uint8_t mux)
{
    return (uint16_t)(SGM58031_CONFIG_START | ((uint16_t)(mux & 7u) << SGM58031_CONFIG_MUX_SHIFT));
}

uint16_t sgm58031_uplink_code(int16_t code)
{
    return code < 0 ? 0u : (uint16_t)code;
}

sgm58031_status_t sgm58031_probe(const sgm58031_bus_t *bus)
{
    uint16_t id;
    if (bus->read(bus->ctx, SGM58031_REG_CHIP_ID, &id) != 0)
    {
        return SGM58031_ERR_BUS;
    }
    return id == SGM58031_CHIP_ID ? SGM58031_OK : SGM58031_ERR_ID;
}

/* Polls CONFIG until OS reads 1 (no conversion running). */
static sgm58031_status_t wait_ready(const sgm58031_bus_t *bus, uint16_t *config)
{
    for (unsigned i = 0; i < SGM58031_POLL_LIMIT; i++)
    {
        if (bus->read(bus->ctx, SGM58031_REG_CONFIG, config) != 0)
        {
            return SGM58031_ERR_BUS;
        }
        if ((*config & SGM58031_CONFIG_OS) != 0u)
        {
            return SGM58031_OK;
        }
        bus->delay_ms(bus->ctx, 1u);
    }
    return SGM58031_ERR_TIMEOUT;
}

sgm58031_status_t sgm58031_measure(const sgm58031_bus_t *bus, uint8_t mux, int16_t *code)
{
    uint16_t config;
    sgm58031_status_t st;

    /* A start request during a conversion is ignored, so let any unfinished
     * conversion end before selecting the channel. */
    st = wait_ready(bus, &config);
    if (st != SGM58031_OK)
    {
        return st;
    }
    if (bus->write(bus->ctx, SGM58031_REG_CONFIG1, SGM58031_CONFIG1_BUS_FLEX) != 0 ||
        bus->write(bus->ctx, SGM58031_REG_CONFIG, sgm58031_config_word(mux)) != 0)
    {
        return SGM58031_ERR_BUS;
    }
    bus->delay_ms(bus->ctx, SGM58031_SETTLE_MS);

    st = wait_ready(bus, &config);
    if (st != SGM58031_OK)
    {
        return st;
    }
    /* Reading too early returns the previous channel's result. */
    if (((config >> SGM58031_CONFIG_MUX_SHIFT) & 7u) != (mux & 7u))
    {
        return SGM58031_ERR_MUX;
    }
    uint16_t raw;
    if (bus->read(bus->ctx, SGM58031_REG_CONVERSION, &raw) != 0)
    {
        return SGM58031_ERR_BUS;
    }
    *code = (int16_t)raw;
    return SGM58031_OK;
}
