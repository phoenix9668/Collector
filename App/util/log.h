/**
 * @file    log.h
 * @brief   Text output on USART1, next to the binary B0/B2 frames.
 *
 * log_status() emits "##<text>##\r\n" status frames. The 4G module, the ESP32
 * adapter and the station parse these, so they are always on.
 * LOG_DEBUG() lines ("[dbg] ...") exist only in APP_DEBUG_LOG builds.
 *
 * Task context only: these block on the USART1 mutex.
 */
#ifndef LOG_H
#define LOG_H

#include "app_config.h"

void log_status(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
void log_raw(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

#if APP_DEBUG_LOG
void log_debug(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
#define LOG_DEBUG(...) log_debug(__VA_ARGS__)
#else
#define LOG_DEBUG(...) \
    do                 \
    {                  \
    } while (0)
#endif

#endif /* LOG_H */
