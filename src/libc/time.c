
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
    pit_set_ticks(ms);
    while (pit_get_ticks() > 0)
        asm("hlt");
}
