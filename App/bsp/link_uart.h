/**
 * @file    link_uart.h
 * @brief   USART1 to the 4G module / ESP32 adapter, 115200 8N1.
 *
 * RX: circular DMA (channel 3) plus the IDLE-line interrupt. A frame is the
 *     bytes received before the line goes idle. Only Link_Task reads frames.
 * TX: DMA (channel 2), serialised by linkTxMutex; any task may write.
 */
#ifndef LINK_UART_H
#define LINK_UART_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Before the scheduler starts: wires the DMA channels and enables USART1. */
void link_uart_init(void);

/* Blocks until sent (task context). False on timeout or mutex timeout. */
bool link_uart_write(const void *data, size_t len);

/* Busy-wait write without RTOS calls, for fatal-error paths only. */
void link_uart_write_polled(const char *text);

/* Link_Task only. Waits up to timeout_ms for a complete frame; returns its
 * length (0 on timeout) and points *frame at an internal buffer that stays
 * valid until the next call. Frames longer than APP_LINK_FRAME_MAX are
 * dropped. */
size_t link_uart_receive(const uint8_t **frame, uint32_t timeout_ms);

#endif /* LINK_UART_H */
