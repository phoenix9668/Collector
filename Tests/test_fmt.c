/**
 * @file    test_fmt.c
 * @brief   fmt_snprintf() against the formats the firmware uses.
 */
#include <limits.h>
#include <stdint.h>

#include "check.h"
#include "fmt.h"

void test_fmt(void)
{
    char buf[64];

    fmt_snprintf(buf, sizeof(buf), "plain");
    CHECK_STR(buf, "plain");
    fmt_snprintf(buf, sizeof(buf), "%d|%i|%u", -42, 7, 42u);
    CHECK_STR(buf, "-42|7|42");
    fmt_snprintf(buf, sizeof(buf), "%d", INT_MIN);
    CHECK_STR(buf, "-2147483648");
    fmt_snprintf(buf, sizeof(buf), "%x %X %08lx %08lX", 0xbeefu, 0xbeefu, 0x0B310001ul, 0xABCul);
    CHECK_STR(buf, "beef BEEF 0b310001 00000ABC");
    fmt_snprintf(buf, sizeof(buf), "%02X%02X", 0x0B, 0x31);
    CHECK_STR(buf, "0B31");
    fmt_snprintf(buf, sizeof(buf), "20%02u-%02u-%02u %02u:%02u:%02u", 26u, 11u, 6u, 2u, 3u, 9u);
    CHECK_STR(buf, "2026-11-06 02:03:09");
    fmt_snprintf(buf, sizeof(buf), "[%5d][%-5d][%05d]", -12, 34, -56);
    CHECK_STR(buf, "[  -12][34   ][-0056]");
    fmt_snprintf(buf, sizeof(buf), "%s|%-6s|%6s|%c|%%", "ab", "cd", "ef", 'Z');
    CHECK_STR(buf, "ab|cd    |    ef|Z|%");
    const char *volatile missing = NULL; /* hide the NULL from -Wformat */
    fmt_snprintf(buf, sizeof(buf), "%s", missing);
    CHECK_STR(buf, "(null)");
    fmt_snprintf(buf, sizeof(buf), "%lu", (unsigned long)UINT32_MAX);
    CHECK_STR(buf, "4294967295");

    /* Truncation: always terminated, returns the full length. */
    char small[6];
    int n = fmt_snprintf(small, sizeof(small), "Collector %s", "2.0.0");
    CHECK_EQ(n, 15);
    CHECK_STR(small, "Colle");
    CHECK_EQ(fmt_snprintf(NULL, 0, "%u", 12345u), 5);
}
