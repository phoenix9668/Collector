/**
 * @file    app_main.h
 * @brief   Application entry points used by the CubeMX-generated code.
 */
#ifndef APP_MAIN_H
#define APP_MAIN_H

#include <stdint.h>

/* Fault codes kept in RTC backup register DR1 across the reset they cause. */
typedef enum
{
    APP_FAULT_NONE = 0,
    APP_FAULT_HAL_ERROR = 1,      /* Error_Handler() from generated init code */
    APP_FAULT_HARDFAULT = 2,
    APP_FAULT_STACK_OVERFLOW = 3,
    APP_FAULT_MALLOC = 4,
} app_fault_t;

/* Called from main() after the MX_xxx_Init() calls, before the kernel starts. */
void App_Init(void);

/* Records the fault, lights LED_GREEN and resets the MCU. Usable from any
 * context, including fault handlers and before the scheduler runs. */
void App_FatalError(app_fault_t fault) __attribute__((noreturn));

/* CollectorID as stored in the data EEPROM, most significant byte first. */
const uint8_t *app_collector_id(void);

/* Housekeeping task, once, APP_BOOT_REPORT_DELAY_MS after power-up:
 * "##Collector <ver> ID=... reset=... fault=...##". */
void app_report_boot(void);

#endif /* APP_MAIN_H */
