/**
 * @file    check.h
 * @brief   Minimal assertion helpers for the host unit tests.
 */
#ifndef CHECK_H
#define CHECK_H

#include <stdio.h>
#include <string.h>

extern int g_checks;
extern int g_failures;

#define CHECK(cond)                                                              \
    do                                                                           \
    {                                                                            \
        g_checks++;                                                              \
        if (!(cond))                                                             \
        {                                                                        \
            g_failures++;                                                        \
            printf("%s:%d: CHECK(%s) failed\n", __FILE__, __LINE__, #cond);     \
        }                                                                        \
    } while (0)

#define CHECK_EQ(a, b)                                                                              \
    do                                                                                              \
    {                                                                                               \
        long long a_ = (long long)(a);                                                              \
        long long b_ = (long long)(b);                                                              \
        g_checks++;                                                                                 \
        if (a_ != b_)                                                                               \
        {                                                                                           \
            g_failures++;                                                                           \
            printf("%s:%d: %s == %s failed: %lld != %lld (0x%llx != 0x%llx)\n", __FILE__, __LINE__, \
                   #a, #b, a_, b_, (unsigned long long)a_, (unsigned long long)b_);                 \
        }                                                                                           \
    } while (0)

#define CHECK_STR(a, b)                                                                   \
    do                                                                                    \
    {                                                                                     \
        const char *a_ = (a); /* evaluate each argument once */                            \
        const char *b_ = (b);                                                             \
        g_checks++;                                                                       \
        if (strcmp(a_, b_) != 0)                                                          \
        {                                                                                 \
            g_failures++;                                                                 \
            printf("%s:%d: \"%s\" != \"%s\"\n", __FILE__, __LINE__, a_, b_);              \
        }                                                                                 \
    } while (0)

#define CHECK_MEM(a, b, n)                                                   \
    do                                                                       \
    {                                                                        \
        g_checks++;                                                          \
        if (memcmp((a), (b), (n)) != 0)                                      \
        {                                                                    \
            g_failures++;                                                    \
            printf("%s:%d: memory %s != %s\n", __FILE__, __LINE__, #a, #b);  \
        }                                                                    \
    } while (0)

void test_fmt(void);
void test_crc32(void);
void test_uplink(void);
void test_downlink(void);
void test_sgm58031(void);
void test_misc(void);
void test_status(void);

/* Frames for Tools/crosscheck_frames.py (written by main.c --dump). */
void dump_frames(FILE *out);

#endif /* CHECK_H */
