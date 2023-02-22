
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

