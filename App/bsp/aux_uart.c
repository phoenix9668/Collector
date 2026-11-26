/**
 * @file    aux_uart.c
 * @brief   LPUART1 interrupt-driven TX (see aux_uart.h).
 */
#include "aux_uart.h"

#include <stdint.h>

#include "app_isr.h"
#include "app_rtos.h"
#include "main.h"

#define AUX_UART        LPUART1
#define AUX_SPIN_LIMIT  20000u

static const uint8_t *volatile s_tx_ptr;
static volatile size_t s_tx_left;
static volatile osThreadId_t s_tx_waiter;
static size_t s_tx_len;

void aux_uart_init(void)
{
    LL_LPUART_Enable(AUX_UART);
    for (uint32_t i = 0; i < AUX_SPIN_LIMIT; i++)
    {
        if (LL_LPUART_IsActiveFlag_TEACK(AUX_UART))
        {
            break;
        }
    }
}

void aux_uart_start(const void *data, size_t len)
{
    s_tx_len = len;
    if (len == 0u)
    {
        return;
    }
    s_tx_ptr = (const uint8_t *)data;
    s_tx_left = len;
    s_tx_waiter = osThreadGetId();
    (void)osThreadFlagsClear(APP_FLAG_AUX_TX_DONE);
    LL_LPUART_EnableIT_TXE(AUX_UART);
}

bool aux_uart_wait(void)
{
    if (s_tx_len == 0u)
    {
        return true;
    }
    /* 115200 baud moves a byte every 87 us. */
    uint32_t timeout_ms = (uint32_t)(s_tx_len / 8u) + 20u;
    uint32_t flags = osThreadFlagsWait(APP_FLAG_AUX_TX_DONE, osFlagsWaitAny, timeout_ms);
    bool ok = !app_flags_error(flags);
    if (!ok)
    {
        LL_LPUART_DisableIT_TXE(AUX_UART);
        LL_LPUART_DisableIT_TC(AUX_UART);
    }
    s_tx_waiter = NULL;
    s_tx_len = 0;
    return ok;
}

void aux_uart_irq(void)
{
    if (LL_LPUART_IsEnabledIT_TXE(AUX_UART) && LL_LPUART_IsActiveFlag_TXE(AUX_UART))
    {
        if (s_tx_left > 0u)
        {
            LL_LPUART_TransmitData8(AUX_UART, *s_tx_ptr);
            s_tx_ptr++;
            s_tx_left--;
        }
        if (s_tx_left == 0u)
        {
            LL_LPUART_DisableIT_TXE(AUX_UART);
            LL_LPUART_EnableIT_TC(AUX_UART);
        }
    }
    if (LL_LPUART_IsEnabledIT_TC(AUX_UART) && LL_LPUART_IsActiveFlag_TC(AUX_UART))
    {
        LL_LPUART_DisableIT_TC(AUX_UART);
        LL_LPUART_ClearFlag_TC(AUX_UART);
        if (s_tx_waiter != NULL)
        {
            (void)osThreadFlagsSet(s_tx_waiter, APP_FLAG_AUX_TX_DONE);
        }
    }
    /* RX is unused; never let a stray overrun keep the IRQ pending. */
    if (LL_LPUART_IsActiveFlag_ORE(AUX_UART))
    {
        LL_LPUART_ClearFlag_ORE(AUX_UART);
    }
}
