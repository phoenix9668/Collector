/**
 * @file    log.c
 * @brief   Text output on USART1 (see log.h).
 */
#include "log.h"

#include <stdarg.h>
#include <string.h>

#include "fmt.h"
#include "link_uart.h"

#define LOG_LINE_MAX 96

static void emit(const char *prefix, const char *suffix, const char *fmt, va_list ap)
{
    char line[LOG_LINE_MAX];
    size_t plen = strlen(prefix);
    size_t slen = strlen(suffix);
    size_t n = plen;

    memcpy(line, prefix, plen);
    /* The body may use what is left after the suffix; fmt adds a NUL. */
    size_t room = sizeof(line) - plen - slen;
    int body = fmt_vsnprintf(&line[n], room, fmt, ap);
    if (body > 0)
    {
        n += ((size_t)body < room) ? (size_t)body : room - 1u; /* truncated */
    }
    memcpy(&line[n], suffix, slen);
    n += slen;
    (void)link_uart_write(line, n);
}

void log_status(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    emit("##", "##\r\n", fmt, ap);
    va_end(ap);
}

void log_raw(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    emit("", "", fmt, ap);
    va_end(ap);
}

#if APP_DEBUG_LOG
void log_debug(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    emit("[dbg] ", "\r\n", fmt, ap);
    va_end(ap);
}
#endif
