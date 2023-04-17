
#include <stdint.h>
#include <time.h>
#include <kernel/pit.h>
#include <kernel/rtc.h>

#define DAYSPER4YEARS    (365 * 4 + 1)
#define CENTURY2YEARS(c) ((c - 1) * 100)

/**
 * @var days
 * @brief Days in each of the 4 year leap cycles. Used by time()
 */
static const uint16_t days_v[4][12] = {
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 },
    { 366, 397, 425, 456, 486, 517, 547, 578, 609, 639, 670, 700 },
    { 731, 762, 790, 821, 851, 882, 912, 943, 974, 1004, 1035, 1065 },
    { 1096, 1127, 1155, 1186, 1216, 1247, 1277, 1308, 1339, 1369, 1400, 1430 },
};

/** @todo: Needs like 900 million seconds more :) */
uint32_t time(void* tloc) {
    (void)tloc; /* Unused */

    DateTime now = rtc_get_datetime();

    const uint16_t year = now.date.y + CENTURY2YEARS(now.date.c);
    const uint16_t mon  = now.date.m - 1; /* -1 to get days[][] idx */
    const uint16_t day  = now.date.d - 1; /* -1 to get days[][] idx */
    const uint16_t hour = now.time.h;
    const uint16_t min  = now.time.m;
    const uint16_t sec  = now.time.s;

    /* Days since epoch. Days in the previous 4 year cycles + days inside the
     * current 4 year cycle. */
    const int days = (year / 4 * DAYSPER4YEARS + days_v[year % 4][mon] + day);
    return ((days * 24 + hour) * 60 + min) * 60 + sec;
}

void sleep(uint32_t sec) {
    /* sec -> ms because the PIT frequency is 1ms. See kernel_main */
    sleep_ms(sec * 1000);
}

void sleep_ms(uint64_t ms) {
    /* No need to translate ms to ticks because 1 tick is 1 ms */
    const uint64_t cur_ticks = pit_get_ticks();
    while (pit_get_ticks() < cur_ticks + ms)
        asm("hlt");
}

/**
 * @var timer_ticks
 * @brief 64 bit variable used to count the PIT ticks since boot.
 */
static uint64_t timer_ticks = 0;

void timer_start(void) {
    timer_ticks = pit_get_ticks();
}

uint64_t timer_stop(void) {
    /* Timer doesn't need to be reset to 0 afer stopping, since we will set it
     * anyway when starting next time. */
    return pit_get_ticks() - timer_ticks;
}
