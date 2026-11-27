/**
 * @file    modem_supervisor.c
 * @brief   4G module power key and heartbeat watchdog (see modem_supervisor.h).
 */
#include "modem_supervisor.h"

#include "app_config.h"
#include "board.h"
#include "cmsis_os2.h"
#include "log.h"

static volatile uint32_t s_last_heartbeat; /* reset timer: also restarts after a reset pulse */
static uint32_t s_next_pwrkey;
/* Only real heartbeats, for the status lamp. */
static volatile bool s_heartbeat_received;
static volatile uint32_t s_last_received;

static void pwrkey_pulse(void)
{
    board_modem_pwrkey(true);
    osDelay(APP_MODEM_PWRKEY_PULSE_MS);
    board_modem_pwrkey(false);
    log_status("LTE Module Power On Complete");
}

void modem_supervisor_start(void)
{
    pwrkey_pulse();
    uint32_t now = osKernelGetTickCount();
    s_last_heartbeat = now;
    s_next_pwrkey = now + APP_MODEM_PWRKEY_PERIOD_MS;
}

void modem_supervisor_on_heartbeat(void)
{
    uint32_t now = osKernelGetTickCount();
    s_last_heartbeat = now;
    s_last_received = now;
    s_heartbeat_received = true;
}

bool modem_supervisor_last_heartbeat(uint32_t *tick_ms)
{
    *tick_ms = s_last_received;
    return s_heartbeat_received;
}

void modem_supervisor_poll(uint32_t now)
{
    /* Signed difference: a heartbeat stored after `now` was read is fine. */
    if ((int32_t)(now - s_last_heartbeat) > (int32_t)APP_MODEM_HEARTBEAT_TIMEOUT_MS)
    {
        log_status("LTE Module Reset (no heartbeat)");
        board_modem_reset(true);
        osDelay(APP_MODEM_RESET_PULSE_MS);
        board_modem_reset(false);
        s_last_heartbeat = osKernelGetTickCount();
    }

    if ((int32_t)(now - s_next_pwrkey) >= 0)
    {
        s_next_pwrkey = now + APP_MODEM_PWRKEY_PERIOD_MS;
        pwrkey_pulse();
    }
}
