/**
 * @file    watchdog.h
 * @brief   IWDG refresh gated on task check-ins.
 *
 * The IWDG (~28 s, LSI/256) is refreshed only while every client has checked
 * in within APP_WDG_CHECKIN_TIMEOUT_MS, so a task that hangs resets the
 * board even when the others keep running.
 */
#ifndef WATCHDOG_H
#define WATCHDOG_H

typedef enum
{
    WDG_RADIO = 0,
    WDG_LINK,
    WDG_HOUSE,
    WDG_CLIENT_COUNT
} wdg_client_t;

void watchdog_checkin(wdg_client_t client);

/* Called once a second by the housekeeping task. */
void watchdog_service(void);

#endif /* WATCHDOG_H */
