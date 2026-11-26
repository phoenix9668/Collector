/**
 * @file    housekeeping.c
 * @brief   Housekeeping_Task: boot report, 4G module supervision, supply
 *          monitoring, the alive LED and the watchdog, on a 1 s cycle.
 */
#include <stdint.h>

#include "app_config.h"
#include "app_main.h"
#include "app_rtos.h"
#include "board.h"
#include "cmsis_os2.h"
#include "log.h"
#include "modem_supervisor.h"
#include "power_monitor.h"
#include "watchdog.h"

#if APP_DEBUG_LOG
#include "FreeRTOS.h"
#include "task.h"
#define STACK_REPORT_PERIOD_MS (10u * 60u * 1000u)
#endif

#define HOUSE_PERIOD_MS 1000u

void Housekeeping_Task(void *argument)
{
    (void)argument;

    app_report_boot();
    modem_supervisor_start();

    uint32_t now = osKernelGetTickCount();
    power_monitor_start(now);
    uint32_t next = now;
    uint32_t next_led = now + APP_LED_BLINK_PERIOD_MS;
#if APP_DEBUG_LOG
    uint32_t next_report = now;
#endif

    for (;;)
    {
        watchdog_checkin(WDG_HOUSE);
        watchdog_service();

        now = osKernelGetTickCount();
        if ((int32_t)(now - next_led) >= 0)
        {
            board_led1_toggle();
            next_led += APP_LED_BLINK_PERIOD_MS;
        }
        modem_supervisor_poll(now);
        power_monitor_poll(now);

#if APP_DEBUG_LOG
        if ((int32_t)(now - next_report) >= 0)
        {
            next_report = now + STACK_REPORT_PERIOD_MS;
            log_debug("free stack B: radio %lu, link %lu, house %lu; heap min free %u",
                      (unsigned long)osThreadGetStackSpace(radioTaskHandle),
                      (unsigned long)osThreadGetStackSpace(linkTaskHandle),
                      (unsigned long)osThreadGetStackSpace(houseTaskHandle),
                      (unsigned)xPortGetMinimumEverFreeHeapSize());
        }
#endif

        next += HOUSE_PERIOD_MS;
        if ((int32_t)(next - osKernelGetTickCount()) <= 0)
        {
            next = osKernelGetTickCount() + HOUSE_PERIOD_MS; /* fell behind after a pulse */
        }
        (void)osDelayUntil(next);
    }
}
