
/**
 * @brief Programmable Interval Timer.
 *
 * See: https://wiki.osdev.org/PIT
 *
 * @file
 */

#include <kernel/pit.h>
#include <kernel/io.h>

void pit_init(uint32_t freq) {
    /* freq should be how many HZs it should wait between sending interrupt. We
     * pass the frequency per second to convert it to HZ (by dividing how many
     * HZs are in a sec) */
    freq = PIT_INTERVAL_TO_FREQ(freq);

    /* Select mode/cmd and flags */
    io_outb(PIT_CHANNEL_CMD, PIT_FLAG_CHANNEL_0 | PIT_FLAG_ACCESS_LOHI |
                               PIT_FLAG_MODE_RATE | PIT_FLAG_BINARY_OFF);

    /* Set reload values to the current ms */
    io_outb(PIT_CHANNEL_0, (uint8_t)(freq & 0xFF));
    io_outb(PIT_CHANNEL_0, (uint8_t)((freq & 0xFF00) >> 8));
}

uint16_t pit_read_count(enum pit_io_ports channel_port,
                        enum pit_cmd_flags channel_flag) {
    uint16_t ret = 0;

    asm("cli");                             /* Disable interrupts */
    io_outb(PIT_CHANNEL_CMD, channel_flag); /* Select current channel */
    ret = io_inb(channel_port);             /* Low byte */
    ret |= io_inb(channel_port) << 8;       /* High byte */
    asm("sti");                             /* Re-enable interrupts */

    return ret;
}

/**
 * @brief Stores the PIT tick count since boot.
 * @details Each tick is supposed to be 1 ms. See call to pit_init() from
 * kernel_main()
 *
 * If the counter was incremented from different places at once (e.g.
 * mutiprocessor), we would need to make this variable volatile.
 */
static uint64_t ticks = 0;

void pit_dec(void) {
    if (ticks > 0)
        ticks--;

    /* Tell CPU that it's okay to resume interrupts. See:
     * https://wiki.osdev.org/Interrupts#From_the_OS.27s_perspective */
    io_outb(0x20, 0x20);
}

void pit_inc(void) {
    ticks++;

    /* Tell CPU that it's okay to resume interrupts. See:
     * https://wiki.osdev.org/Interrupts#From_the_OS.27s_perspective */
    io_outb(0x20, 0x20);
}

void pit_set_ticks(uint64_t num) {
    ticks = num;
}

uint64_t pit_get_ticks(void) {
    return ticks;
}
