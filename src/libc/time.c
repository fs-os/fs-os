
#include <stdint.h>
#include <time.h>
#include <kernel/pit.h>
#include <kernel/rtc.h>

#define CENTURY2YEARS(c) ((c - 1) * 100)
#define YEAR2SEC(x)      (x * 31556926)
#define MON2SEC(x)       (x * 2629743)
#define DAY2SEC(x)       (x * 86400)
#define HOUR2SEC(x)      (x * 3600)
#define MIN2SEC(x)       (x * 60)

/** @todo: Needs like 900 million seconds more :) */
uint32_t time(void* tloc) {
    (void)tloc; /* Unused */

    DateTime now = rtc_get_datetime();

    /** @todo: Remove random calibrations */
    const uint16_t year = now.date.y + CENTURY2YEARS(now.date.c) - 1970;
    const uint16_t mon  = now.date.m - 1;
    const uint16_t day  = now.date.d - 2;
    const uint16_t hour = now.time.h - 3;
    const uint16_t min  = now.time.m - 30;
    const uint16_t sec  = now.time.s;

    return YEAR2SEC(year) + MON2SEC(mon) + DAY2SEC(day) + HOUR2SEC(hour) +
           MIN2SEC(min) + sec;
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
