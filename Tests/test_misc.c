/**
 * @file    test_misc.c
 * @brief   Small pure helpers: time validation and EEPROM offset checks.
 */
#include "check.h"
#include "collector_time.h"
#include "nvm.h"

void test_misc(void)
{
    collector_time_t t = {26, 11, 26, 4, 2, 30, 59};
    CHECK(collector_time_valid(&t));
    t.month = 13;
    CHECK(!collector_time_valid(&t));
    t.month = 12;
    t.weekday = 0;
    CHECK(!collector_time_valid(&t));
    t.weekday = 7;
    t.hours = 24;
    CHECK(!collector_time_valid(&t));
    t.hours = 23;
    t.date = 0;
    CHECK(!collector_time_valid(&t));
    t.date = 31;
    t.year = 100;
    CHECK(!collector_time_valid(&t));

    CHECK(nvm_offset_valid(0x0000));
    CHECK(nvm_offset_valid(0x0010));
    CHECK(nvm_offset_valid(0x07FC));
    CHECK(!nvm_offset_valid(0x0800));
    CHECK(!nvm_offset_valid(0x0002));
    CHECK(!nvm_offset_valid(0xFFFC));
}
