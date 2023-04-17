
#include <stdint.h>
#include <time.h>
#include <kernel/pit.h>
#include <kernel/rtc.h>

#define MIN2SEC(x)  (x * 60)
#define HOUR2SEC(x) (x * 3600)
#define DAY2SEC(x)  (x * 86400)
#define MON2SEC(x)  (x * 2629743)

/**
 * @def YEAR2SEC
 * @brief Generates seconds that passed since epoch to 1/1/x, x being a year
 * minus 1900 (1975 -> 75; 2023 -> 123)
 *
 * @details It adds in a day for each year that follows a leap year starting
 * with the first leap year since the Epoch. The first term adds a day every 4
 * years starting in 1973, the second subtracts a day back out every 100 years
 * starting in 2001, and the third adds a day back in every 400 years starting
 * in 2001. The divisions in the formula are integer divisions; that is, the
 * remainder is discarded leaving only the integer quotient.
 *
 * Source:
 * https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap04.html#tag_04_16
 */
#define YEAR2SEC(x)                                                           \
    ((x - 70) * 31536000 + ((x - 69) / 4) * 86400 - ((x - 1) / 100) * 86400 + \
     ((x + 299) / 400) * 86400)

/** @todo: Needs like 900 million seconds more :) */
uint32_t time(void* tloc) {
    (void)tloc; /* Unused */

    DateTime now = rtc_get_datetime();

    /** @todo: Fix month offset */
    const uint32_t sec  = now.time.s;
    const uint32_t min  = MIN2SEC(now.time.m);
    const uint32_t hour = HOUR2SEC(now.time.h);
    const uint32_t day  = DAY2SEC(now.date.d);
    const uint32_t mon  = MON2SEC(now.date.m);
    const uint32_t year = YEAR2SEC(now.date.y + 100); /* (years since 1900) */

    return sec + min + hour + day + mon + year;
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
