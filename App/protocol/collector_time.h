/**
 * @file    collector_time.h
 * @brief   Calendar time as it travels in B0 frames and A8 commands.
 */
#ifndef COLLECTOR_TIME_H
#define COLLECTOR_TIME_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint8_t year;    /* 0..99, years since 2000 */
    uint8_t month;   /* 1..12 */
    uint8_t date;    /* 1..31 */
    uint8_t weekday; /* 1 = Monday .. 7 = Sunday */
    uint8_t hours;   /* 0..23 */
    uint8_t minutes; /* 0..59 */
    uint8_t seconds; /* 0..59 */
} collector_time_t;

static inline bool collector_time_valid(const collector_time_t *t)
{
    return t->year <= 99u && t->month >= 1u && t->month <= 12u && t->date >= 1u && t->date <= 31u &&
           t->weekday >= 1u && t->weekday <= 7u && t->hours <= 23u && t->minutes <= 59u && t->seconds <= 59u;
}

#endif /* COLLECTOR_TIME_H */
