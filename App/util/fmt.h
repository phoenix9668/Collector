/**
 * @file    fmt.h
 * @brief   Small, reentrant snprintf replacement for log lines.
 *
 * Supports %% %c %s %d %i %u %x %X with the '0' and '-' flags, a field width
 * and the 'l' length modifier (int and long are both 32 bits here). It keeps
 * newlib's printf, its heap use and its global state out of the firmware.
 */
#ifndef FMT_H
#define FMT_H

#include <stdarg.h>
#include <stddef.h>

/* Like vsnprintf: always NUL-terminates when size > 0 and returns the length
 * the full output would have had. */
int fmt_vsnprintf(char *buf, size_t size, const char *fmt, va_list ap);
int fmt_snprintf(char *buf, size_t size, const char *fmt, ...) __attribute__((format(printf, 3, 4)));

#endif /* FMT_H */
