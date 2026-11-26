/**
 * @file    test_sgm58031.c
 * @brief   SGM58031 driver against a register model, after the reference
 *          tests in Collector-QuecPython (tests/test_sensors.py, 4.1.6).
 */
#include <stdbool.h>
#include <stdint.h>

#include "check.h"
#include "sgm58031.h"

#define I2C_OP_US 300u /* one register access at 100 kHz */

typedef struct
{
    uint16_t chip_id;
    int16_t inputs[8];  /* conversion result per MUX */
    uint32_t conv_us;   /* single-shot conversion time */
    uint32_t now_us;
    bool busy;
    uint32_t done_at_us;
    uint8_t mux;        /* MUX of the running / last conversion */
    uint16_t config;    /* CONFIG without OS */
    uint16_t conversion;
    int fail_at_op;     /* make the n-th bus access fail (-1: never) */
    int ops;
    bool stuck;         /* never finishes a conversion */
    bool report_wrong_mux;
    struct
    {
        uint8_t reg;
        uint16_t value;
    } writes[16];
    int nwrites;
} model_t;

static void model_init(model_t *m, int16_t battery, int16_t power, uint32_t conv_us)
{
    memset(m, 0, sizeof(*m));
    m->chip_id = SGM58031_CHIP_ID;
    m->inputs[SGM58031_MUX_AIN2_AIN3] = battery;
    m->inputs[SGM58031_MUX_AIN0_AIN1] = power;
    m->conv_us = conv_us;
    m->fail_at_op = -1;
}

static void model_update(model_t *m)
{
    if (m->busy && !m->stuck && m->now_us >= m->done_at_us)
    {
        m->busy = false;
        m->conversion = (uint16_t)m->inputs[m->mux];
    }
}

static int model_read(void *ctx, uint8_t reg, uint16_t *value)
{
    model_t *m = ctx;
    m->now_us += I2C_OP_US;
    if (m->ops++ == m->fail_at_op)
    {
        return -1;
    }
    model_update(m);
    switch (reg)
    {
    case SGM58031_REG_CHIP_ID:
        *value = m->chip_id;
        break;
    case SGM58031_REG_CONFIG:
    {
        uint8_t mux = m->report_wrong_mux ? (uint8_t)((m->mux + 1u) & 7u) : m->mux;
        *value = (uint16_t)((m->config & ~(0x7000u | SGM58031_CONFIG_OS)) | ((uint16_t)mux << 12) |
                            (m->busy ? 0u : SGM58031_CONFIG_OS));
        break;
    }
    case SGM58031_REG_CONVERSION:
        *value = m->conversion;
        break;
    default:
        *value = 0;
        break;
    }
    return 0;
}

static int model_write(void *ctx, uint8_t reg, uint16_t value)
{
    model_t *m = ctx;
    m->now_us += I2C_OP_US;
    if (m->ops++ == m->fail_at_op)
    {
        return -1;
    }
    model_update(m);
    if (m->nwrites < 16)
    {
        m->writes[m->nwrites].reg = reg;
        m->writes[m->nwrites].value = value;
        m->nwrites++;
    }
    /* A start request during a conversion is ignored. */
    if (reg == SGM58031_REG_CONFIG && !m->busy)
    {
        m->config = value & (uint16_t)~SGM58031_CONFIG_OS;
        if (value & SGM58031_CONFIG_OS)
        {
            m->mux = (uint8_t)((value >> 12) & 7u);
            m->busy = true;
            m->done_at_us = m->now_us + m->conv_us;
        }
    }
    return 0;
}

static void model_delay(void *ctx, uint32_t ms)
{
    model_t *m = ctx;
    m->now_us += ms * 1000u;
}

static sgm58031_bus_t bus_for(model_t *m)
{
    sgm58031_bus_t bus = {model_write, model_read, model_delay, m};
    return bus;
}

void test_sgm58031(void)
{
    model_t m;
    sgm58031_bus_t bus;
    int16_t code;

    CHECK_EQ(sgm58031_config_word(SGM58031_MUX_AIN2_AIN3), 0xB3E3);
    CHECK_EQ(sgm58031_config_word(SGM58031_MUX_AIN0_AIN1), 0x83E3);
    CHECK_EQ(sgm58031_uplink_code(-1), 0);
    CHECK_EQ(sgm58031_uplink_code(INT16_MIN), 0);
    CHECK_EQ(sgm58031_uplink_code(7705), 7705);
    CHECK_EQ(sgm58031_uplink_code(INT16_MAX), 32767);

    /* Probe. */
    model_init(&m, 0, 0, 1200);
    bus = bus_for(&m);
    CHECK_EQ(sgm58031_probe(&bus), SGM58031_OK);
    m.chip_id = 0x0040;
    CHECK_EQ(sgm58031_probe(&bus), SGM58031_ERR_ID);
    m.fail_at_op = m.ops;
    CHECK_EQ(sgm58031_probe(&bus), SGM58031_ERR_BUS);

    /* Register sequence: battery on AIN2-AIN3, power on AIN0-AIN1;
     * Config1 only sets BUS_FLEX. */
    model_init(&m, 16384, 16384, 1200);
    bus = bus_for(&m);
    CHECK_EQ(sgm58031_measure(&bus, SGM58031_MUX_AIN2_AIN3, &code), SGM58031_OK);
    CHECK_EQ(code, 16384);
    CHECK_EQ(sgm58031_measure(&bus, SGM58031_MUX_AIN0_AIN1, &code), SGM58031_OK);
    CHECK_EQ(m.nwrites, 4);
    CHECK_EQ(m.writes[0].reg, 0x04);
    CHECK_EQ(m.writes[0].value, 0x0010);
    CHECK_EQ(m.writes[1].reg, 0x01);
    CHECK_EQ(m.writes[1].value, 0xB3E3);
    CHECK_EQ(m.writes[2].reg, 0x04);
    CHECK_EQ(m.writes[2].value, 0x0010);
    CHECK_EQ(m.writes[3].reg, 0x01);
    CHECK_EQ(m.writes[3].value, 0x83E3);

    /* Unit 062 on battery: AIN2 7705, AIN0 13. A fixed 5 ms wait used to
     * return the battery code as power once a conversion took longer. */
    static const uint32_t conv_times_us[] = {3200, 5000, 6000, 30000};
    for (size_t i = 0; i < sizeof(conv_times_us) / sizeof(conv_times_us[0]); i++)
    {
        model_init(&m, 7705, 13, conv_times_us[i]);
        bus = bus_for(&m);
        CHECK_EQ(sgm58031_measure(&bus, SGM58031_MUX_AIN2_AIN3, &code), SGM58031_OK);
        CHECK_EQ(code, 7705);
        CHECK_EQ(sgm58031_measure(&bus, SGM58031_MUX_AIN0_AIN1, &code), SGM58031_OK);
        CHECK_EQ(code, 13);
        CHECK_EQ(sgm58031_measure(&bus, SGM58031_MUX_AIN2_AIN3, &code), SGM58031_OK);
        CHECK_EQ(code, 7705);
    }

    /* An unfinished battery conversion is waited out before switching. */
    model_init(&m, 7705, 13, 6000);
    m.busy = true;
    m.mux = SGM58031_MUX_AIN2_AIN3;
    m.done_at_us = 6000;
    bus = bus_for(&m);
    CHECK_EQ(sgm58031_measure(&bus, SGM58031_MUX_AIN0_AIN1, &code), SGM58031_OK);
    CHECK_EQ(code, 13);

    /* Negative codes come back signed; the uplink clamps them. */
    model_init(&m, -3, 0, 1200);
    bus = bus_for(&m);
    CHECK_EQ(sgm58031_measure(&bus, SGM58031_MUX_AIN2_AIN3, &code), SGM58031_OK);
    CHECK_EQ(code, -3);
    CHECK_EQ(sgm58031_uplink_code(code), 0);

    /* Failures: stuck converter, wrong MUX, bus errors at every step. */
    model_init(&m, 1, 2, 1200);
    m.stuck = true;
    m.busy = true;
    bus = bus_for(&m);
    CHECK_EQ(sgm58031_measure(&bus, SGM58031_MUX_AIN0_AIN1, &code), SGM58031_ERR_TIMEOUT);

    model_init(&m, 1, 2, 1200);
    m.report_wrong_mux = true;
    bus = bus_for(&m);
    CHECK_EQ(sgm58031_measure(&bus, SGM58031_MUX_AIN0_AIN1, &code), SGM58031_ERR_MUX);

    /* A measurement is 5 accesses: read CONFIG, write CONFIG1, write CONFIG,
     * read CONFIG (done), read CONVERSION. */
    for (int fail = 0; fail < 5; fail++)
    {
        model_init(&m, 1, 2, 1200);
        m.fail_at_op = fail;
        bus = bus_for(&m);
        CHECK_EQ(sgm58031_measure(&bus, SGM58031_MUX_AIN0_AIN1, &code), SGM58031_ERR_BUS);
    }
}
