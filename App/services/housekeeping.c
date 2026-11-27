/**
 * @file    housekeeping.c
 * @brief   Housekeeping_Task: boot report, 4G module supervision, supply
 *          monitoring, the status lamp's fault evaluation and the watchdog,
 *          on a 1 s cycle.
 */
#include <stdbool.h>
#include <stdint.h>

#include "app_config.h"
#include "app_main.h"
#include "app_rtos.h"
#include "board.h"
#include "cmsis_os2.h"
#include "log.h"
#include "modem_supervisor.h"
#include "power_monitor.h"
#include "station_status.h"
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

    station_status_start(); /* lamp blinks "booting" from here on */
    modem_supervisor_start();

    uint32_t now = osKernelGetTickCount();
    power_monitor_start(now);
    uint32_t next = now;
    bool boot_reported = false;
#if APP_DEBUG_LOG
    uint32_t next_report = now;
#endif

    for (;;)
    {
        watchdog_checkin(WDG_HOUSE);
        watchdog_service();

        now = osKernelGetTickCount();
        if (!boot_reported && now >= APP_BOOT_REPORT_DELAY_MS)
        {
            /* Late enough for the 4G module / ESP32 adapter to be listening. */
            app_report_boot();
            boot_reported = true;
        }
        modem_supervisor_poll(now);
        power_monitor_poll(now);
        station_status_tick(osKernelGetTickCount());

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
