/**
 * @file    link_uart.c
 * @brief   USART1 link to the 4G module / ESP32 adapter (see link_uart.h).
 *
 * Collector.ioc sets up USART1 (LL), DMA1 channel 3 (RX, circular) and
 * channel 2 (TX, normal) and enables USART1 in MX_USART1_UART_Init().
 * link_uart_init() stops it again, wires the DMA channels and restarts it.
 */
#include "link_uart.h"

#include <string.h>

#include "app_config.h"
#include "app_isr.h"
#include "app_rtos.h"
#include "main.h"

#define LINK_UART        USART1
#define LINK_DMA         DMA1
#define LINK_DMA_RX      LL_DMA_CHANNEL_3
#define LINK_DMA_TX      LL_DMA_CHANNEL_2
#define LINK_RX_DMA_SIZE 64u
#define LINK_MUTEX_TIMEOUT_MS 1000u
#define LINK_SPIN_LIMIT  20000u

static uint8_t s_rx_dma[LINK_RX_DMA_SIZE];
static size_t s_rx_read; /* next unread index in s_rx_dma */

static uint8_t s_frame[APP_LINK_FRAME_MAX];
static size_t s_frame_len;
static bool s_frame_overflow;
static bool s_frame_handed_out;

static volatile osThreadId_t s_tx_waiter;

static void notify(osThreadId_t task, uint32_t flags)
{
    if (task != NULL)
    {
        (void)osThreadFlagsSet(task, flags);
    }
}

static void rx_dma_start(void)
{
    LL_DMA_DisableChannel(LINK_DMA, LINK_DMA_RX);
    LL_DMA_ClearFlag_GI3(LINK_DMA);
    LL_DMA_SetPeriphAddress(LINK_DMA, LINK_DMA_RX,
                            LL_USART_DMA_GetRegAddr(LINK_UART, LL_USART_DMA_REG_DATA_RECEIVE));
    LL_DMA_SetMemoryAddress(LINK_DMA, LINK_DMA_RX, (uint32_t)s_rx_dma);
    LL_DMA_SetDataLength(LINK_DMA, LINK_DMA_RX, LINK_RX_DMA_SIZE);
    LL_DMA_EnableIT_HT(LINK_DMA, LINK_DMA_RX);
    LL_DMA_EnableIT_TC(LINK_DMA, LINK_DMA_RX);
    s_rx_read = 0;
    LL_DMA_EnableChannel(LINK_DMA, LINK_DMA_RX);
}

void link_uart_init(void)
{
    LL_USART_Disable(LINK_UART);
    /* A late DMA read then only loses a byte instead of stalling RX. */
    LL_USART_DisableOverrunDetect(LINK_UART);

    LL_DMA_DisableChannel(LINK_DMA, LINK_DMA_TX);
    LL_DMA_ClearFlag_GI2(LINK_DMA);
    LL_DMA_SetPeriphAddress(LINK_DMA, LINK_DMA_TX,
                            LL_USART_DMA_GetRegAddr(LINK_UART, LL_USART_DMA_REG_DATA_TRANSMIT));
    LL_DMA_EnableIT_TC(LINK_DMA, LINK_DMA_TX);

    rx_dma_start();
    LL_USART_EnableDMAReq_RX(LINK_UART);
    LL_USART_EnableDMAReq_TX(LINK_UART);
    LL_USART_ClearFlag_IDLE(LINK_UART);
    LL_USART_EnableIT_IDLE(LINK_UART);
    LL_USART_Enable(LINK_UART);

    for (uint32_t i = 0; i < LINK_SPIN_LIMIT; i++)
    {
        if (LL_USART_IsActiveFlag_TEACK(LINK_UART) && LL_USART_IsActiveFlag_REACK(LINK_UART))
        {
            break;
        }
    }
}

/* ---------------------------------------------------------------- TX --- */

bool link_uart_write(const void *data, size_t len)
{
    if (len == 0u)
    {
        return true;
    }
    if (osMutexAcquire(linkTxMutexHandle, LINK_MUTEX_TIMEOUT_MS) != osOK)
    {
        return false;
    }

    s_tx_waiter = osThreadGetId();
    (void)osThreadFlagsClear(APP_FLAG_LINK_TX_DONE);

    LL_DMA_DisableChannel(LINK_DMA, LINK_DMA_TX);
    LL_DMA_ClearFlag_GI2(LINK_DMA);
    LL_DMA_SetMemoryAddress(LINK_DMA, LINK_DMA_TX, (uint32_t)data);
    LL_DMA_SetDataLength(LINK_DMA, LINK_DMA_TX, (uint32_t)len);
    LL_USART_ClearFlag_TC(LINK_UART);
    LL_DMA_EnableChannel(LINK_DMA, LINK_DMA_TX);

    /* 115200 baud moves a byte every 87 us. */
    uint32_t timeout_ms = (uint32_t)(len / 8u) + 20u;
    uint32_t flags = osThreadFlagsWait(APP_FLAG_LINK_TX_DONE, osFlagsWaitAny, timeout_ms);
    bool ok = !app_flags_error(flags);
    if (ok)
    {
        /* DMA is done once the last byte is in TDR; wait for it to leave. */
        for (uint32_t i = 0; i < LINK_SPIN_LIMIT && !LL_USART_IsActiveFlag_TC(LINK_UART); i++)
        {
        }
    }
    LL_DMA_DisableChannel(LINK_DMA, LINK_DMA_TX);
    s_tx_waiter = NULL;

    (void)osMutexRelease(linkTxMutexHandle);
    return ok;
}

void link_uart_write_polled(const char *text)
{
    LL_DMA_DisableChannel(LINK_DMA, LINK_DMA_TX);
    for (const char *p = text; *p != '\0'; p++)
    {
        for (uint32_t i = 0; i < LINK_SPIN_LIMIT && !LL_USART_IsActiveFlag_TXE(LINK_UART); i++)
        {
        }
        LL_USART_TransmitData8(LINK_UART, (uint8_t)*p);
    }
    for (uint32_t i = 0; i < LINK_SPIN_LIMIT && !LL_USART_IsActiveFlag_TC(LINK_UART); i++)
    {
    }
}

/* ---------------------------------------------------------------- RX --- */

static void frame_append(uint8_t c)
{
    if (s_frame_len < sizeof(s_frame))
    {
        s_frame[s_frame_len++] = c;
    }
    else
    {
        s_frame_overflow = true;
    }
}

static void rx_drain(void)
{
    size_t pos = LINK_RX_DMA_SIZE - LL_DMA_GetDataLength(LINK_DMA, LINK_DMA_RX);
    if (pos >= LINK_RX_DMA_SIZE)
    {
        pos = 0;
    }
    while (s_rx_read != pos)
    {
        frame_append(s_rx_dma[s_rx_read]);
        s_rx_read = (s_rx_read + 1u) % LINK_RX_DMA_SIZE;
    }
}

static void rx_health_check(void)
{
    if (!LL_DMA_IsEnabledChannel(LINK_DMA, LINK_DMA_RX))
    {
        /* A DMA transfer error disables the channel: start again. */
        rx_dma_start();
    }
    if (LL_USART_IsActiveFlag_FE(LINK_UART))
    {
        LL_USART_ClearFlag_FE(LINK_UART);
    }
    if (LL_USART_IsActiveFlag_NE(LINK_UART))
    {
        LL_USART_ClearFlag_NE(LINK_UART);
    }
}

size_t link_uart_receive(const uint8_t **frame, uint32_t timeout_ms)
{
    if (s_frame_handed_out)
    {
        s_frame_len = 0;
        s_frame_overflow = false;
        s_frame_handed_out = false;
    }

    uint32_t flags = osThreadFlagsWait(APP_FLAG_LINK_RX | APP_FLAG_LINK_IDLE, osFlagsWaitAny, timeout_ms);
    if (app_flags_error(flags))
    {
        rx_health_check();
        return 0;
    }
    rx_drain();
    if ((flags & APP_FLAG_LINK_IDLE) == 0u)
    {
        return 0; /* more to come */
    }

    s_frame_handed_out = true;
    *frame = s_frame;
    return s_frame_overflow ? 0u : s_frame_len;
}

/* ---------------------------------------------------------- interrupts --- */

void link_uart_irq(void)
{
    if (LL_USART_IsEnabledIT_IDLE(LINK_UART) && LL_USART_IsActiveFlag_IDLE(LINK_UART))
    {
        LL_USART_ClearFlag_IDLE(LINK_UART);
        notify(linkTaskHandle, APP_FLAG_LINK_RX | APP_FLAG_LINK_IDLE);
    }
}

void link_uart_dma_irq(void)
{
    if (LL_DMA_IsEnabledIT_HT(LINK_DMA, LINK_DMA_RX) && LL_DMA_IsActiveFlag_HT3(LINK_DMA))
    {
        LL_DMA_ClearFlag_HT3(LINK_DMA);
        notify(linkTaskHandle, APP_FLAG_LINK_RX);
    }
    if (LL_DMA_IsEnabledIT_TC(LINK_DMA, LINK_DMA_RX) && LL_DMA_IsActiveFlag_TC3(LINK_DMA))
    {
        LL_DMA_ClearFlag_TC3(LINK_DMA);
        notify(linkTaskHandle, APP_FLAG_LINK_RX);
    }
    if (LL_DMA_IsEnabledIT_TC(LINK_DMA, LINK_DMA_TX) && LL_DMA_IsActiveFlag_TC2(LINK_DMA))
    {
        LL_DMA_ClearFlag_TC2(LINK_DMA);
        notify(s_tx_waiter, APP_FLAG_LINK_TX_DONE);
    }
}
