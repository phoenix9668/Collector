/**
 * @file    aux_uart.h
 * @brief   LPUART1 (PB10 TX), 115200 8N1, interrupt-driven TX only.
 *
 * Carries a copy of every B0 frame (APP_B0_MIRROR_LPUART1). Only the radio
 * task writes here, so there is no mutex.
 */
#ifndef AUX_UART_H
#define AUX_UART_H

#include <stdbool.h>
#include <stddef.h>

/* Before the scheduler starts. */
void aux_uart_init(void);

/* Starts sending in the background; data must stay valid until
 * aux_uart_wait() returns. Lets the caller drive USART1 at the same time. */
void aux_uart_start(const void *data, size_t len);

/* Blocks the task that called aux_uart_start() until the last byte is out;
 * false on timeout. */
bool aux_uart_wait(void);

#endif /* AUX_UART_H */
