
/* PC Speaker. See: https://wiki.osdev.org/PC_Speaker */

#include <stdint.h>
#include <time.h>
#include <kernel/pit.h>
#include <kernel/io.h>
#include <kernel/pcspkr.h>

/* pcspkr_play: play pc speaker with "freq" frequency (1s format) */
void pcspkr_play(uint32_t freq) {
    /* freq should be how many HZs it should wait bewteen sending interrupt. We pass
     * the frequency per second to convert it to HZ (by dividing how many HZs are in
     * a sec) */
    freq = PIT_INTERVAL_TO_FREQ(freq);

    /* Set up the PIT */
    io_outb(PIT_CHANNEL_CMD, PIT_FLAG_CHANNEL_2 | PIT_FLAG_ACCESS_LOHI |
                               PIT_FLAG_MODE_RATE | PIT_FLAG_BINARY_OFF);
    io_outb(PIT_CHANNEL_2, (uint8_t)(freq & 0xFF));
    io_outb(PIT_CHANNEL_2, (uint8_t)((freq & 0xFF00) >> 8));

    uint8_t tmp = io_inb(PCSPKR_PORT);

    /* This is not the same as (!(tmp & 3)) */
    if (tmp != (tmp | 3))
        /* Tell the PC speaker to listen PIT channel 2 for freq. If one of the last 2
         * bits is 0, set them to 1. */
        io_outb(PCSPKR_PORT, tmp | 0x3);
}

/* pcspkr_clear: tell the pc speaker to stop listening from PIT channel 2 */
void pcspkr_clear(void) {
    /* Set last 2 bits to 0 */
    uint8_t tmp = io_inb(PCSPKR_PORT) & 0xFC;
    io_outb(PCSPKR_PORT, tmp);
}

/* pcspkr_beep: simple beep using the pc speaker */
void pcspkr_beep(void) {
    pcspkr_play(1000);
    sleep_ms(20);
    pcspkr_clear();

    /* TODO: Reset old frequency of PIT channel 2. Not needed for now since we don't
     * use channel 2. */
}

