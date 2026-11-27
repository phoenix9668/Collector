/**
 * @file    main.c
 * @brief   Host unit tests for the pure-C parts of App/.
 *
 * usage: collector_tests                 run all tests
 *        collector_tests --dump <file>   also write sample B0/B2 frames as hex
 *                                        for Tools/crosscheck_frames.py
 */
#include <stdio.h>
#include <string.h>

#include "check.h"

int g_checks;
int g_failures;

int main(int argc, char **argv)
{
    test_fmt();
    test_crc32();
    test_uplink();
    test_downlink();
    test_sgm58031();
    test_misc();
    test_status();

    if (argc == 3 && strcmp(argv[1], "--dump") == 0)
    {
        FILE *out = fopen(argv[2], "w");
        if (out == NULL)
        {
            printf("cannot write %s\n", argv[2]);
            return 2;
        }
        dump_frames(out);
        fclose(out);
    }

    printf("%d checks, %d failures\n", g_checks, g_failures);
    return g_failures == 0 ? 0 : 1;
}
