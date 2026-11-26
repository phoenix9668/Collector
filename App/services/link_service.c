/**
 * @file    link_service.c
 * @brief   Link_Task: handles what the 4G module / ESP32 adapter sends on
 *          USART1 (formats in protocol/downlink.h).
 *
 * The "##...##" replies keep the wording of the original firmware so tools
 * that look for them keep working.
 */
#include <stdbool.h>
#include <stdint.h>

#include "app_config.h"
#include "app_main.h"
#include "app_rtos.h"
#include "board.h"
#include "cmsis_os2.h"
#include "downlink.h"
#include "link_uart.h"
#include "log.h"
#include "modem_supervisor.h"
#include "nvm.h"
#include "rtc_clock.h"
#include "watchdog.h"

#define LINK_WAIT_MS 1000u

static void do_memory(const dl_command_t *cmd)
{
    uint32_t offset = cmd->address;

    if (!nvm_offset_valid(offset))
    {
        log_status("Memory Address Error");
        return;
    }
    switch (cmd->op)
    {
    case DL_MEM_WRITE:
        /* A new CollectorID (offset 0) takes effect after the next reset. */
        log_status(nvm_write_word(offset, cmd->data) ? "Write Memory Complete" : "Write Memory Error");
        break;
    case DL_MEM_READ:
    {
        uint32_t value = 0;
        (void)nvm_read_word(offset, &value);
        log_raw("%08lx\n", (unsigned long)value);
        log_status("Read Memory Complete");
        break;
    }
    case DL_MEM_ERASE:
        log_status(nvm_write_word(offset, 0u) ? "Erase Memory Complete" : "Erase Memory Error");
        break;
    default:
        log_status("Memory Op Unused");
        break;
    }
}

static void do_function(const dl_command_t *cmd)
{
    switch (cmd->op)
    {
    case DL_FN_RESET:
        log_status("System Reset"); /* returns once the bytes are on the wire */
        NVIC_SystemReset();
        break;
    case DL_FN_SET_TIME:
        if (cmd->has_time && rtc_clock_set(&cmd->time))
        {
            log_status("Time Set Complete");
        }
        else
        {
            log_status("Time Format Error");
        }
        break;
    case DL_FN_READ_TIME:
    {
        collector_time_t t;
        (void)rtc_clock_get(&t);
        log_status("RTC 20%02u-%02u-%02u W%u %02u:%02u:%02u", t.year, t.month, t.date, t.weekday, t.hours,
                   t.minutes, t.seconds);
        break;
    }
    default:
        log_status("FunctionID Unused");
        break;
    }
}

static void handle_frame(const uint8_t *frame, size_t len)
{
    if (downlink_has_heartbeat(frame, len))
    {
        modem_supervisor_on_heartbeat();
        LOG_DEBUG("link: heartbeat");
    }

    dl_command_t cmd;
    switch (downlink_parse(frame, len, app_collector_id(), &cmd))
    {
    case DL_MEMORY:
        log_status("Header Right");
        do_memory(&cmd);
        break;
    case DL_FUNCTION:
        log_status("CollectorID Right");
        do_function(&cmd);
        break;
    default:
        break;
    }
}

void Link_Task(void *argument)
{
    (void)argument;

    for (;;)
    {
        watchdog_checkin(WDG_LINK);

        const uint8_t *frame = NULL;
        size_t len = link_uart_receive(&frame, LINK_WAIT_MS);
        if (len == 0u)
        {
            continue;
        }
        board_led2(true);
        handle_frame(frame, len);
        board_led2(false);
    }
}
