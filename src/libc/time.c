
#include <stdint.h>
#include <time.h>
#include <kernel/pit.h>
#include <kernel/rtc.h>

uint32_t time(void* tloc) {
    (void)tloc; /* Unused */

    DateTime now = rtc_get_datetime();

    /**
     * @todo Search mktime function from glibc or something:
     * https://www.epochconverter.com/programming/c
     * */
    return now.time.s;
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
