/**
 * @file    fmt.c
 * @brief   Small, reentrant snprintf replacement (see fmt.h).
 */
#include "fmt.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    char *buf;
    size_t size;
    size_t len;
} out_t;

static void put(out_t *o, char c)
{
    if (o->len + 1u < o->size)
    {
        o->buf[o->len] = c;
    }
    o->len++;
}

static void pad(out_t *o, char c, int count)
{
    while (count-- > 0)
    {
        put(o, c);
    }
}

static void put_field(out_t *o, const char *s, int n, int width, bool left, bool zero, bool negative)
{
    int total = n + (negative ? 1 : 0);
    int fill = width > total ? width - total : 0;

    if (!left && !zero)
    {
        pad(o, ' ', fill);
    }
    if (negative)
    {
        put(o, '-');
    }
    if (!left && zero)
    {
        pad(o, '0', fill);
    }
    for (int i = 0; i < n; i++)
    {
        put(o, s[i]);
    }
    if (left)
    {
        pad(o, ' ', fill);
    }
}

static int utoa_rev(uint32_t v, unsigned base, bool upper, char *tmp)
{
    const char *digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";
    int n = 0;
    do
    {
        tmp[n++] = digits[v % base];
        v /= base;
    } while (v != 0u);
    return n;
}

int fmt_vsnprintf(char *buf, size_t size, const char *fmt, va_list ap)
{
    out_t o = {buf, size, 0u};

    for (const char *p = fmt; *p != '\0'; p++)
    {
        if (*p != '%')
        {
            put(&o, *p);
            continue;
        }
        p++;
        bool left = false;
        bool zero = false;
        for (;; p++)
        {
            if (*p == '-')
            {
                left = true;
            }
            else if (*p == '0')
            {
                zero = true;
            }
            else
            {
                break;
            }
        }
        int width = 0;
        while (*p >= '0' && *p <= '9')
        {
            width = width * 10 + (*p - '0');
            p++;
        }
        bool is_long = false;
        while (*p == 'l' || *p == 'h')
        {
            is_long = is_long || (*p == 'l');
            p++;
        }

        char tmp[11];
        char num[11];
        switch (*p)
        {
        case '%':
            put(&o, '%');
            break;
        case 'c':
            num[0] = (char)va_arg(ap, int);
            put_field(&o, num, 1, width, left, false, false);
            break;
        case 's':
        {
            const char *s = va_arg(ap, const char *);
            if (s == NULL)
            {
                s = "(null)";
            }
            int n = 0;
            while (s[n] != '\0')
            {
                n++;
            }
            put_field(&o, s, n, width, left, false, false);
            break;
        }
        case 'd':
        case 'i':
        {
            int32_t v = is_long ? (int32_t)va_arg(ap, long) : (int32_t)va_arg(ap, int);
            bool neg = v < 0;
            uint32_t mag = neg ? (uint32_t)0u - (uint32_t)v : (uint32_t)v;
            int n = utoa_rev(mag, 10u, false, tmp);
            for (int i = 0; i < n; i++)
            {
                num[i] = tmp[n - 1 - i];
            }
            put_field(&o, num, n, width, left, zero && !left, neg);
            break;
        }
        case 'u':
        case 'x':
        case 'X':
        {
            uint32_t v = is_long ? (uint32_t)va_arg(ap, unsigned long) : (uint32_t)va_arg(ap, unsigned int);
            unsigned base = (*p == 'u') ? 10u : 16u;
            int n = utoa_rev(v, base, *p == 'X', tmp);
            for (int i = 0; i < n; i++)
            {
                num[i] = tmp[n - 1 - i];
            }
            put_field(&o, num, n, width, left, zero && !left, false);
            break;
        }
        case '\0':
            p--; /* lone '%' at the end: stop */
            break;
        default:
            put(&o, '%');
            put(&o, *p);
            break;
        }
    }

    if (size > 0u)
    {
        buf[o.len < size ? o.len : size - 1u] = '\0';
    }
    return (int)o.len;
}

int fmt_snprintf(char *buf, size_t size, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int n = fmt_vsnprintf(buf, size, fmt, ap);
    va_end(ap);
    return n;
}
