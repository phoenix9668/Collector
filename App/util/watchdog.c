/**
 * @file    watchdog.c
 * @brief   IWDG refresh gated on task check-ins (see watchdog.h).
 */
#include "watchdog.h"

#include "app_config.h"
#include "cmsis_os2.h"
#include "iwdg.h"
#include "log.h"

/* Ticks of the last check-in. 0 at boot counts as a check-in at t = 0. */
static volatile uint32_t s_last_checkin[WDG_CLIENT_COUNT];

void watchdog_checkin(wdg_client_t client)
{
    if (client < WDG_CLIENT_COUNT)
    {
        s_last_checkin[client] = osKernelGetTickCount();
    }
}

void watchdog_service(void)
{
    uint32_t now = osKernelGetTickCount();

    for (unsigned i = 0; i < WDG_CLIENT_COUNT; i++)
    {
        if (now - s_last_checkin[i] > APP_WDG_CHECKIN_TIMEOUT_MS)
        {
            LOG_DEBUG("wdg: client %u silent, IWDG left to expire", i);
            return;
        }
    }
    (void)HAL_IWDG_Refresh(&hiwdg);
}
