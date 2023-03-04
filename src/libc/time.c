
#include <stdint.h>
#include <time.h>
#include <kernel/pit.h>

/* sleep: sleep "sec" seconds */
void sleep(uint32_t sec) {
    /* sec -> ms because the PIT frequency is 1ms. See kernel_main */
    sleep_ms(sec * 1000);
}

/* sleep_ms: sleep "ms" milliseconds */
void sleep_ms(uint64_t ms) {
    /* No need to translate ms to ticks because 1 tick is 1 ms */
    const uint64_t cur_ticks = pit_get_ticks();
    while (pit_get_ticks() < cur_ticks + ms)
        asm("hlt");
}

static uint64_t timer_ticks = 0;

/* timer_start: starts the pit timer. Calling multiple times resets the count */
void timer_start(void) {
    timer_ticks = pit_get_ticks();
}

/* timer_stop: returns the ms difference from the current tick since the tick we
 * started the timer (time in ms that passed since we called timer_start). */
uint64_t timer_stop(void) {
    /* Timer doesn't need to be reset to 0 afer stopping, since we will set it anyway
     * when starting next time. */
    return pit_get_ticks() - timer_ticks;
}

