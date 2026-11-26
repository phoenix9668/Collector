/**
 * @file    cc1101.c
 * @brief   CC1101 register access and setup (see cc1101.h).
 *
 * SPI1 runs at 4 MHz (Collector.ioc), inside the CC1101's 6.5 MHz limit for
 * burst access without inter-byte delays. Every wait on the bus is bounded.
 */
#include "cc1101.h"

#include <stddef.h>

#include "board.h"
#include "main.h"

#define CC1101_SPI        SPI1
#define CC1101_SPI_LIMIT  2000u
#define CC1101_MISO_LIMIT_MS 10u

/* PA table: 10, 7, 5, 0, -5, -10, -15, -20 dBm (TX is not used). */
static const uint8_t k_pa_table[8] = {0xC0, 0xC8, 0x84, 0x60, 0x34, 0x1D, 0x0E, 0x12};

/* 433.0 MHz, GFSK, 76.767 kBaud, deviation 31.7 kHz, RX BW 232 kHz,
 * Manchester, whitening, variable length, CRC on, append status,
 * address check with 0x00/0xFF broadcast. SYNC, ADDR and MDMCFG1
 * (24-byte preamble) are set after this table. */
static const uint8_t k_config[][2] = {
    {CC1101_IOCFG2, 0x29},   {CC1101_IOCFG1, 0x2E},   {CC1101_IOCFG0, 0x46},   {CC1101_FIFOTHR, 0x4E},
    {CC1101_SYNC1, 0xD3},    {CC1101_SYNC0, 0x91},    {CC1101_PKTLEN, 0xFF},   {CC1101_PKTCTRL1, 0x07},
    {CC1101_PKTCTRL0, 0x45}, {CC1101_ADDR, 0x00},     {CC1101_CHANNR, 0x00},   {CC1101_FSCTRL1, 0x0B},
    {CC1101_FSCTRL0, 0x00},  {CC1101_FREQ2, 0x10},    {CC1101_FREQ1, 0xA7},    {CC1101_FREQ0, 0x62},
    {CC1101_MDMCFG4, 0x7B},  {CC1101_MDMCFG3, 0x83},  {CC1101_MDMCFG2, 0x9B},  {CC1101_MDMCFG1, 0x22},
    {CC1101_MDMCFG0, 0xF8},  {CC1101_DEVIATN, 0x42},  {CC1101_MCSM2, 0x03},    {CC1101_MCSM1, 0x30},
    {CC1101_MCSM0, 0x18},    {CC1101_FOCCFG, 0x1D},   {CC1101_BSCFG, 0x1C},    {CC1101_AGCCTRL2, 0xC7},
    {CC1101_AGCCTRL1, 0x00}, {CC1101_AGCCTRL0, 0xB2}, {CC1101_WOREVT1, 0x8C},  {CC1101_WOREVT0, 0xA0},
    {CC1101_WORCTRL, 0xF8},  {CC1101_FREND1, 0xB6},   {CC1101_FREND0, 0x10},   {CC1101_FSCAL3, 0xEA},
    {CC1101_FSCAL2, 0x2A},   {CC1101_FSCAL1, 0x00},   {CC1101_FSCAL0, 0x1F},   {CC1101_RCCTRL1, 0x41},
    {CC1101_RCCTRL0, 0x00},  {CC1101_FSTEST, 0x59},   {CC1101_PTEST, 0x7F},    {CC1101_AGCTEST, 0x3F},
    {CC1101_TEST2, 0x81},    {CC1101_TEST1, 0x35},    {CC1101_TEST0, 0x09},
};

#define CC1101_MDMCFG1_PREAMBLE_24 0x72u
#define CC1101_IOCFG0_RX           0x46u /* sync / end of packet, inverted */
#define CC1101_IOCFG2_RX           0x40u /* RX FIFO threshold, inverted */

static bool s_spi_error;

static uint8_t spi_xfer(uint8_t out)
{
    uint32_t i;
    for (i = 0; i < CC1101_SPI_LIMIT && !LL_SPI_IsActiveFlag_TXE(CC1101_SPI); i++)
    {
    }
    LL_SPI_TransmitData8(CC1101_SPI, out);
    for (i = 0; i < CC1101_SPI_LIMIT && !LL_SPI_IsActiveFlag_RXNE(CC1101_SPI); i++)
    {
    }
    if (i == CC1101_SPI_LIMIT)
    {
        s_spi_error = true;
        return 0xFFu;
    }
    return LL_SPI_ReceiveData8(CC1101_SPI);
}

static void write_reg(uint8_t addr, uint8_t value)
{
    board_rf_cs(true);
    (void)spi_xfer(addr);
    (void)spi_xfer(value);
    board_rf_cs(false);
}

static void write_burst(uint8_t addr, const uint8_t *data, uint8_t len)
{
    board_rf_cs(true);
    (void)spi_xfer(addr | CC1101_WRITE_BURST);
    for (uint8_t i = 0; i < len; i++)
    {
        (void)spi_xfer(data[i]);
    }
    board_rf_cs(false);
}

static uint8_t read_status(uint8_t addr)
{
    board_rf_cs(true);
    (void)spi_xfer(addr | CC1101_READ_BURST);
    uint8_t v = spi_xfer(0xFF);
    board_rf_cs(false);
    return v;
}

static void strobe(uint8_t cmd)
{
    board_rf_cs(true);
    (void)spi_xfer(cmd);
    board_rf_cs(false);
}

/* With CSn low the CC1101 holds MISO high until its crystal is stable. */
static bool wait_chip_ready(void)
{
    uint32_t start = HAL_GetTick();
    while (board_rf_miso_high())
    {
        if (HAL_GetTick() - start > CC1101_MISO_LIMIT_MS)
        {
            return false;
        }
    }
    return true;
}

static bool reset_chip(void)
{
    /* Manual power-up reset sequence (CC1101 datasheet 19.1.2). */
    board_rf_cs(false);
    board_rf_cs(true);
    HAL_Delay(1);
    board_rf_cs(false);
    HAL_Delay(1); /* > 40 us */
    board_rf_cs(true);
    bool ok = wait_chip_ready();
    (void)spi_xfer(CC1101_SRES);
    ok = wait_chip_ready() && ok;
    board_rf_cs(false);
    HAL_Delay(1);
    return ok;
}

bool cc1101_init(uint8_t address, uint16_t sync_word)
{
    if (!LL_SPI_IsEnabled(CC1101_SPI))
    {
        LL_SPI_Enable(CC1101_SPI);
    }
    s_spi_error = false;
    bool ok = reset_chip();

    for (size_t i = 0; i < sizeof(k_config) / sizeof(k_config[0]); i++)
    {
        write_reg(k_config[i][0], k_config[i][1]);
    }
    /* PKTCTRL1 from the table already selects address check + broadcast. */
    write_reg(CC1101_ADDR, address);
    write_reg(CC1101_SYNC1, (uint8_t)(sync_word >> 8));
    write_reg(CC1101_SYNC0, (uint8_t)sync_word);
    write_reg(CC1101_MDMCFG1, CC1101_MDMCFG1_PREAMBLE_24);
    write_burst(CC1101_PATABLE, k_pa_table, (uint8_t)sizeof(k_pa_table));

    /* A chip that answers reports a version other than 0x00 / 0xFF
     * (0x14 for current CC1101 silicon). */
    uint8_t version = read_status(CC1101_VERSION);
    return ok && !s_spi_error && version != 0x00u && version != 0xFFu;
}

void cc1101_rx_start(void)
{
    board_rf_path_rx();
    write_reg(CC1101_IOCFG0, CC1101_IOCFG0_RX);
    write_reg(CC1101_IOCFG2, CC1101_IOCFG2_RX);
    strobe(CC1101_SRX);
}

void cc1101_rx_flush(void)
{
    board_rf_path_off();
    strobe(CC1101_SIDLE);
    strobe(CC1101_SFRX);
}

uint8_t cc1101_rx_bytes(void)
{
    uint8_t prev;
    uint8_t now = read_status(CC1101_RXBYTES);
    for (int i = 0; i < 8; i++)
    {
        prev = now;
        now = read_status(CC1101_RXBYTES);
        if (now == prev)
        {
            break;
        }
    }
    return now;
}

void cc1101_read_fifo(uint8_t *buf, uint8_t len)
{
    if (len == 0u)
    {
        return;
    }
    board_rf_cs(true);
    (void)spi_xfer(CC1101_RXFIFO | CC1101_READ_BURST);
    for (uint8_t i = 0; i < len; i++)
    {
        buf[i] = spi_xfer(0xFF);
    }
    board_rf_cs(false);
}

uint8_t cc1101_read_fifo_byte(void)
{
    board_rf_cs(true);
    (void)spi_xfer(CC1101_RXFIFO | CC1101_READ_SINGLE);
    uint8_t v = spi_xfer(0xFF);
    board_rf_cs(false);
    return v;
}

uint8_t cc1101_marcstate(void)
{
    return read_status(CC1101_MARCSTATE) & CC1101_MARCSTATE_MASK;
}

int16_t cc1101_rssi_dbm(uint8_t raw)
{
    int16_t v = (raw >= 128u) ? (int16_t)((int16_t)raw - 256) : (int16_t)raw;
    return (int16_t)(v / 2 - 74);
}
