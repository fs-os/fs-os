
#include <stdint.h>
#include <time.h>
#include <kernel/pit.h>
#include <kernel/rtc.h>

#define MIN2SEC(x)  ((x)*60)
#define HOUR2SEC(x) ((x)*3600)
#define DAY2SEC(x)  ((x)*86400)
#define MON2SEC(x)  ((x)*2629743)
#define YEAR2SEC(x) ((x)*31556926)

uint32_t time(void* tloc) {
    (void)tloc; /* Unused */

    DateTime now = rtc_get_datetime();

    /** @todo: Fix month offset */
    const uint32_t sec  = now.time.s;
    const uint32_t min  = MIN2SEC(now.time.m);
    const uint32_t hour = HOUR2SEC(now.time.h);
    const uint32_t day  = DAY2SEC(now.date.d);
    const uint32_t mon  = MON2SEC(now.date.m);
    const uint32_t year = YEAR2SEC(now.date.y + 30); /* (years since 1970) */

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
