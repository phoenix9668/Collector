/**
 * @file    app_isr.h
 * @brief   Interrupt entry points called from the USER CODE sections of
 *          Core/Src/stm32l0xx_it.c. Each one only clears flags and signals a
 *          task; no logging or blocking calls in interrupt context.
 */
#ifndef APP_ISR_H
#define APP_ISR_H

#include "app_main.h"

void radio_gdo0_irq(void);    /* EXTI line 3, PA3 = CC1101 GDO0 */
void radio_gdo2_irq(void);    /* EXTI line 4, PA4 = CC1101 GDO2 */
void link_uart_irq(void);     /* USART1 (IDLE line)             */
void link_uart_dma_irq(void); /* DMA1 channel 2 (TX) / 3 (RX)    */
void aux_uart_irq(void);      /* LPUART1 TX                     */

#endif /* APP_ISR_H */
