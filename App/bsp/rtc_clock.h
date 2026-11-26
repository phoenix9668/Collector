/**
 * @file    rtc_clock.h
 * @brief   RTC calendar access and the RTC backup registers the app uses.
 *
 * DR0 = RTC_CLOCK_VALID_MAGIC once the calendar was set after power-up.
 *       MX_RTC_Init() (Core/Src/rtc.c, USER CODE Check_RTC_BKUP) then keeps
 *       the running calendar across resets instead of setting 2000-01-01.
 * DR1 = fault code of the last fatal reset (app_fault_t), cleared at boot.
 * The backup domain survives resets but not a loss of VDD.
 */
#ifndef RTC_CLOCK_H
#define RTC_CLOCK_H

#include <stdbool.h>

#include "collector_time.h"
#include "main.h"

#define RTC_CLOCK_BKP_VALID   RTC_BKP_DR0
#define RTC_CLOCK_BKP_FAULT   RTC_BKP_DR1
#define RTC_CLOCK_VALID_MAGIC 0x32F2u

bool rtc_clock_get(collector_time_t *t);
bool rtc_clock_set(const collector_time_t *t);

#endif /* RTC_CLOCK_H */
