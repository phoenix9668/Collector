/**
 * @file    app_rtos.h
 * @brief   RTOS objects created by CubeMX (Collector.ioc -> freertos.c) and
 *          the thread-flag bits the application uses with them.
 *
 * The tasks and the mutex are defined in Collector.ioc (static allocation,
 * code generation "As external"); their bodies live in App/services.
 */
#ifndef APP_RTOS_H
#define APP_RTOS_H

#include "cmsis_os2.h"

extern osThreadId_t radioTaskHandle; /* Radio_Task,        osPriorityHigh        */
extern osThreadId_t linkTaskHandle;  /* Link_Task,         osPriorityAboveNormal */
extern osThreadId_t houseTaskHandle; /* Housekeeping_Task, osPriorityNormal      */
extern osMutexId_t linkTxMutexHandle; /* serialises USART1 TX */

/* Task bodies (App/services); freertos.c starts them by these names. */
void Radio_Task(void *argument);
void Link_Task(void *argument);
void Housekeeping_Task(void *argument);

/* Thread flags. Every bit has one meaning across all tasks. */
#define APP_FLAG_RF_START      (1u << 0) /* GDO0 asserted: sync word found      */
#define APP_FLAG_RF_END        (1u << 1) /* GDO0 released: packet finished       */
#define APP_FLAG_RF_FIFO       (1u << 2) /* GDO2: RX FIFO reached its threshold */
#define APP_FLAG_RF_ALL        (APP_FLAG_RF_START | APP_FLAG_RF_END | APP_FLAG_RF_FIFO)

#define APP_FLAG_LINK_RX       (1u << 4) /* USART1 DMA half/full transfer       */
#define APP_FLAG_LINK_IDLE     (1u << 5) /* USART1 line idle: frame complete     */

#define APP_FLAG_LINK_TX_DONE  (1u << 8) /* USART1 DMA TX finished              */
#define APP_FLAG_AUX_TX_DONE   (1u << 9) /* LPUART1 TX finished                 */

/* osThreadFlagsWait() returns an error code with the top bit set. */
static inline int app_flags_error(uint32_t flags)
{
    return (flags & 0x80000000u) != 0u;
}

/* Waits for any of `flags`; use instead of osThreadFlagsWait().
 * FreeRTOS wakes a notification wait only for a notification sent after the
 * task's previous wait returned, so a flag set while the task was blocked
 * waiting for other flags (e.g. RF_START during a USART1 send) would otherwise
 * sit unseen. Querying the value counts as a notification and closes that gap. */
static inline uint32_t app_flags_wait(uint32_t flags, uint32_t timeout_ms)
{
    (void)osThreadFlagsGet();
    return osThreadFlagsWait(flags, osFlagsWaitAny, timeout_ms);
}

#endif /* APP_RTOS_H */
