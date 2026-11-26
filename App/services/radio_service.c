/**
 * @file    radio_service.c
 * @brief   Radio_Task: receives collar packets on the CC1101 and forwards each
 *          valid one as a B0 frame on USART1 (and LPUART1).
 *
 * The task owns SPI1 and the CC1101 exclusively. GDO edges only wake it up;
 * how much to read always comes from RXBYTES, so a missed or extra edge can
 * at worst cost one poll interval.
 */
#include <stdbool.h>
#include <stdint.h>

#include "app_config.h"
#include "app_isr.h"
#include "app_main.h"
#include "app_rtos.h"
#include "aux_uart.h"
#include "board.h"
#include "cc1101.h"
#include "link_uart.h"
#include "log.h"
#include "rtc_clock.h"
#include "uplink.h"
#include "watchdog.h"

#define RF_PAYLOAD_MAX 255u
#define RF_POLL_MS     5u
#define RF_IDLE_WAIT_MS 1000u

typedef enum
{
    RF_RX_OK,
    RF_RX_EMPTY,    /* packet ended without data for us (address filter, length 0) */
    RF_RX_TIMEOUT,
    RF_RX_OVERFLOW,
} rf_rx_result_t;

typedef struct
{
    uint8_t len; /* payload bytes after the address byte */
    uint8_t addr;
    uint8_t rssi;
    bool hw_crc_ok;
} rf_packet_t;

static uint8_t s_payload[RF_PAYLOAD_MAX];
static uint8_t s_frame[RF_PAYLOAD_MAX + UPLINK_B0_OVERHEAD];

/* ------------------------------------------------------------ interrupts */

void radio_gdo0_irq(void)
{
    if (radioTaskHandle != NULL)
    {
        /* GDO0 is inverted: low while a packet is on air. */
        (void)osThreadFlagsSet(radioTaskHandle, board_rf_gdo0_high() ? APP_FLAG_RF_END : APP_FLAG_RF_START);
    }
}

void radio_gdo2_irq(void)
{
    if (radioTaskHandle != NULL)
    {
        (void)osThreadFlagsSet(radioTaskHandle, APP_FLAG_RF_FIFO);
    }
}

static void gdo_irq_enable(bool enable)
{
    if (enable)
    {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_3 | LL_EXTI_LINE_4);
        NVIC_ClearPendingIRQ(GDO0_EXTI_IRQn);
        NVIC_ClearPendingIRQ(GDO2_EXTI_IRQn);
        NVIC_EnableIRQ(GDO0_EXTI_IRQn);
        NVIC_EnableIRQ(GDO2_EXTI_IRQn);
    }
    else
    {
        /* After power-up GDO0 outputs a clock until configured. */
        NVIC_DisableIRQ(GDO0_EXTI_IRQn);
        NVIC_DisableIRQ(GDO2_EXTI_IRQn);
    }
}

/* ------------------------------------------------------------ helpers */

static void sleep_with_checkin(uint32_t ms)
{
    while (ms > 0u)
    {
        uint32_t step = ms > 1000u ? 1000u : ms;
        watchdog_checkin(WDG_RADIO);
        osDelay(step);
        ms -= step;
    }
    watchdog_checkin(WDG_RADIO);
}

static void restart_rx(void)
{
    cc1101_rx_flush();
    (void)osThreadFlagsClear(APP_FLAG_RF_ALL);
    cc1101_rx_start();
}

/* Power-cycles and configures the CC1101, then starts RX. */
static bool radio_bring_up(void)
{
    gdo_irq_enable(false);
    board_rf_path_off();
    board_rf_power(false);
    osDelay(APP_RF_POWER_OFF_MS);
    board_rf_power(true);
    bool ok = cc1101_init(APP_RF_ADDRESS, APP_RF_SYNC_WORD);
    cc1101_rx_flush();
    (void)osThreadFlagsClear(APP_FLAG_RF_ALL);
    gdo_irq_enable(true);
    cc1101_rx_start();
    return ok;
}

/* Waits for the next GDO edge, at most RF_POLL_MS. False once past deadline. */
static bool wait_rf(uint32_t deadline)
{
    int32_t left = (int32_t)(deadline - osKernelGetTickCount());
    if (left <= 0)
    {
        return false;
    }
    uint32_t t = (uint32_t)left < RF_POLL_MS ? (uint32_t)left : RF_POLL_MS;
    (void)app_flags_wait(APP_FLAG_RF_END | APP_FLAG_RF_FIFO, t);
    return true;
}

/* ------------------------------------------------------------ receive */

static rf_rx_result_t rf_receive(rf_packet_t *pkt)
{
    uint32_t deadline = osKernelGetTickCount() + APP_RF_PACKET_TIMEOUT_MS;
    uint8_t n;

    /* Length and address bytes. */
    for (;;)
    {
        n = cc1101_rx_bytes();
        if ((n & CC1101_RXBYTES_OVERFLOW) != 0u)
        {
            return RF_RX_OVERFLOW;
        }
        if ((n & CC1101_RXBYTES_COUNT) >= 2u)
        {
            break;
        }
        if (board_rf_gdo0_high() && (cc1101_rx_bytes() & CC1101_RXBYTES_COUNT) < 2u)
        {
            return RF_RX_EMPTY;
        }
        if (!wait_rf(deadline))
        {
            return RF_RX_TIMEOUT;
        }
    }
    uint8_t len_byte = cc1101_read_fifo_byte();
    pkt->addr = cc1101_read_fifo_byte();
    if (len_byte == 0u)
    {
        return RF_RX_EMPTY;
    }
    pkt->len = (uint8_t)(len_byte - 1u); /* the length byte counts the address */

    /* Payload: while the packet is still arriving leave one byte in the FIFO
     * (CC1101 errata: the last byte may be read corrupt while being written). */
    uint8_t got = 0;
    while (got < pkt->len)
    {
        n = cc1101_rx_bytes();
        if ((n & CC1101_RXBYTES_OVERFLOW) != 0u)
        {
            return RF_RX_OVERFLOW;
        }
        n &= CC1101_RXBYTES_COUNT;
        bool ended = board_rf_gdo0_high();
        uint8_t avail = ended ? n : (n > 0u ? (uint8_t)(n - 1u) : 0u);
        uint8_t want = (uint8_t)(pkt->len - got);
        if (avail > want)
        {
            avail = want;
        }
        if (avail > 0u)
        {
            cc1101_read_fifo(&s_payload[got], avail);
            got = (uint8_t)(got + avail);
            continue;
        }
        if (!wait_rf(deadline))
        {
            return RF_RX_TIMEOUT;
        }
    }

    /* Appended status: RSSI, then CRC_OK | LQI. */
    while ((cc1101_rx_bytes() & CC1101_RXBYTES_COUNT) < 2u)
    {
        if (!wait_rf(deadline))
        {
            return RF_RX_TIMEOUT;
        }
    }
    uint8_t status[2];
    cc1101_read_fifo(status, sizeof(status));
    pkt->rssi = status[0];
    pkt->hw_crc_ok = (status[1] & CC1101_LQI_CRC_OK) != 0u;
    return RF_RX_OK;
}

static void forward_b0(const rf_packet_t *pkt)
{
    collector_time_t now;
    (void)rtc_clock_get(&now);
    size_t n = uplink_build_b0(s_frame, sizeof(s_frame), app_collector_id(), s_payload, pkt->len, pkt->rssi, &now);
    if (n == 0u)
    {
        return;
    }
    board_led1(true);
#if APP_B0_MIRROR_LPUART1
    aux_uart_start(s_frame, n);
#endif
    (void)link_uart_write(s_frame, n);
#if APP_B0_MIRROR_LPUART1
    (void)aux_uart_wait();
#endif
    board_led1(false);
    LOG_DEBUG("rf: B0 sent, payload %u B, RSSI %d dBm", pkt->len, cc1101_rssi_dbm(pkt->rssi));
}

static void handle_packet(void)
{
    rf_packet_t pkt = {0};

    (void)osThreadFlagsClear(APP_FLAG_RF_END | APP_FLAG_RF_FIFO);
    rf_rx_result_t res = rf_receive(&pkt);
    /* Listen again before the slow part, so the next packet is not lost. */
    restart_rx();

    switch (res)
    {
    case RF_RX_OK:
        if (!pkt.hw_crc_ok)
        {
            LOG_DEBUG("rf: receive crc error");
        }
        else if (!uplink_rf_crc_ok(s_payload, pkt.len))
        {
            LOG_DEBUG("rf: payload CRC-32 mismatch, %u B", pkt.len);
        }
        else
        {
            forward_b0(&pkt);
        }
        break;
    case RF_RX_EMPTY:
        LOG_DEBUG("rf: receive error or address filtering fail");
        break;
    case RF_RX_TIMEOUT:
        LOG_DEBUG("rf: packet timeout");
        break;
    case RF_RX_OVERFLOW:
        LOG_DEBUG("rf: RX FIFO overflow");
        break;
    default:
        break;
    }
}

/* Puts the radio back into RX if it fell out of it (e.g. after an edge
 * that never reached the task). */
static void keep_listening(void)
{
    uint8_t state = cc1101_marcstate();
    if (state == CC1101_STATE_IDLE || state == CC1101_STATE_RXFIFO_OVERFLOW)
    {
        LOG_DEBUG("rf: state 0x%02x, restarting RX", state);
        restart_rx();
    }
}

/* ------------------------------------------------------------ task */

void Radio_Task(void *argument)
{
    (void)argument;

    /* Give the 4G module time to boot before the first B0 frame. */
    sleep_with_checkin(APP_RF_START_DELAY_MS);
    while (!radio_bring_up())
    {
        log_status("CC1101 init error");
        sleep_with_checkin(APP_RF_RETRY_MS);
    }
    uint32_t reinit_due = osKernelGetTickCount() + APP_RF_REINIT_PERIOD_MS;

    for (;;)
    {
        watchdog_checkin(WDG_RADIO);

        if ((int32_t)(osKernelGetTickCount() - reinit_due) >= 0)
        {
            bool ok = radio_bring_up();
            if (!ok)
            {
                log_status("CC1101 init error");
            }
            reinit_due = osKernelGetTickCount() + (ok ? APP_RF_REINIT_PERIOD_MS : APP_RF_RETRY_MS);
        }

        uint32_t flags = app_flags_wait(APP_FLAG_RF_START, RF_IDLE_WAIT_MS);
        if (app_flags_error(flags))
        {
            keep_listening();
            continue;
        }
        handle_packet();
    }
}
