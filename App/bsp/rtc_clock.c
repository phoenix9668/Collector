/**
 * @file    rtc_clock.c
 * @brief   RTC calendar access (see rtc_clock.h).
 */
#include "rtc_clock.h"

#include "cmsis_os2.h"
#include "rtc.h"

bool rtc_clock_get(collector_time_t *t)
{
    RTC_TimeTypeDef time = {0};
    RTC_DateTypeDef date = {0};

    /* GetTime locks the shadow registers until GetDate; keep the pair atomic. */
    int32_t lock = osKernelLock();
    bool ok = HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN) == HAL_OK;
    ok = (HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN) == HAL_OK) && ok;
    (void)osKernelRestoreLock(lock);

    t->year = date.Year;
    t->month = date.Month;
    t->date = date.Date;
    t->weekday = date.WeekDay;
    t->hours = time.Hours;
    t->minutes = time.Minutes;
    t->seconds = time.Seconds;
    return ok;
}

bool rtc_clock_set(const collector_time_t *t)
{
    if (!collector_time_valid(t))
    {
        return false;
    }
    RTC_TimeTypeDef time = {0};
    RTC_DateTypeDef date = {0};
    time.Hours = t->hours;
    time.Minutes = t->minutes;
    time.Seconds = t->seconds;
    time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    time.StoreOperation = RTC_STOREOPERATION_RESET;
    date.Year = t->year;
    date.Month = t->month;
    date.Date = t->date;
    date.WeekDay = t->weekday;

    int32_t lock = osKernelLock();
    bool ok = HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN) == HAL_OK;
    ok = ok && (HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN) == HAL_OK);
    if (ok)
    {
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_CLOCK_BKP_VALID, RTC_CLOCK_VALID_MAGIC);
    }
    (void)osKernelRestoreLock(lock);
    return ok;
}
