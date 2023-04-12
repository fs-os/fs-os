
/**
 * @brief PC Speaker.
 *
 * See: https://wiki.osdev.org/PC_Speaker
 *
 * @file
 */

#include <stdint.h>
#include <time.h>
#include <kernel/pit.h>
#include <kernel/io.h>
#include <kernel/pcspkr.h>

/**
 * @brief Current frequency used by the pc speaker.
 *
 * 0 means disabled.
 */
static uint32_t current_freq = 0;

void pcspkr_play(uint32_t freq) {
    /* Save the current frequency */
    current_freq = freq;

    /* freq should be how many HZs it should wait between sending interrupt. We
     * pass the frequency per second to convert it to HZ (by dividing how many
     * HZs are in a sec) */
    freq = PIT_INTERVAL_TO_FREQ(freq);

    /* Set up the PIT */
    io_outb(PIT_CHANNEL_CMD, PIT_FLAG_CHANNEL_2 | PIT_FLAG_ACCESS_LOHI |
                               PIT_FLAG_MODE_RATE | PIT_FLAG_BINARY_OFF);
    io_outb(PIT_CHANNEL_2, (uint8_t)(freq & 0xFF));
    io_outb(PIT_CHANNEL_2, (uint8_t)((freq & 0xFF00) >> 8));

    uint8_t tmp = io_inb(PCSPKR_PORT);

    /* This is not the same as (!(tmp & 3)) */
    if (tmp != (tmp | 3))
        /* Tell the PC speaker to listen PIT channel 2 for freq. If one of the
         * last 2 bits is 0, set them to 1. */
        io_outb(PCSPKR_PORT, tmp | 0x3);
}

void pcspkr_clear(void) {
    /* Set last 2 bits to 0 */
    uint8_t tmp = io_inb(PCSPKR_PORT) & 0xFC;
    io_outb(PCSPKR_PORT, tmp);

    /* Set the last freq to 0 */
    current_freq = 0;
}

uint32_t pcspkr_get_freq(void) {
    return current_freq;
}

void pcspkr_beep_custom(Beep info) {
    pcspkr_play(info.freq);
    sleep_ms(info.ms_len);
    pcspkr_clear();

    /**
     * @todo Reset old frequency of PIT channel 2. Not needed for now since we
     * don't use channel 2. */
}

