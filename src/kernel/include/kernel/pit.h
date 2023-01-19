
#ifndef _KERNEL_PIT_H
#define _KERNEL_PIT_H

#include <stdint.h>

/* How many times the PIT oscillates in 1 sec by default */
#define PIT_BASE_FREQ 1193182

/* Frequency in 1 sec (e.g. 1000 times in a sec == 1ms) to PIT hz */
#define PIT_INTERVAL_TO_FREQ(freq) (PIT_BASE_FREQ / freq)

enum pit_io_ports {
    PIT_CHANNEL_0   = 0x40, /* time.h functions (sleep, etc.) */
    PIT_CHANNEL_1   = 0x41, /* unused */
    PIT_CHANNEL_2   = 0x42, /* pc speaker */
    PIT_CHANNEL_CMD = 0x43, /* configuring frequencies */
};

enum pit_cmd_flags {
    PIT_FLAG_CHANNEL_0 = 0x00, /* 00000000 */
    PIT_FLAG_CHANNEL_1 = 0x40, /* 01000000 */
    PIT_FLAG_CHANNEL_2 = 0x80, /* 10000000 */
    PIT_FLAG_CHANNEL_3 = 0xC0, /* 11000000 */

    PIT_FLAG_ACCESS_COUNT = 0x00, /* 00000000 */
    PIT_FLAG_ACCESS_LO    = 0x10, /* 00010000 */
    PIT_FLAG_ACCESS_HI    = 0x20, /* 00100000 */
    PIT_FLAG_ACCESS_LOHI  = 0x30, /* 00110000 */

    PIT_FLAG_MODE_INT       = 0x00, /* 00000000 */
    PIT_FLAG_MODE_HW_RET    = 0x02, /* 00000010 */
    PIT_FLAG_MODE_SQUARE    = 0x04, /* 00000100 */
    PIT_FLAG_MODE_RATE      = 0x06, /* 00000110 */
    PIT_FLAG_MODE_SW_STROBE = 0x08, /* 00001000 */
    PIT_FLAG_MODE_HW_STROBE = 0x0A, /* 00001010 */
    PIT_FLAG_MODE_RATE_2    = 0x0C, /* 00001100. Same as rate */
    PIT_FLAG_MODE_SQUARE_2  = 0x0E, /* 00001110. Same as square */

    PIT_FLAG_BINARY_OFF = 0x0, /* 00000000 */
    PIT_FLAG_BINARY_BCD = 0x1, /* 00000001 */
};

/* pit_init: initialize the programmable interval timer with the specified frequency
 * in interrupts per seccond (freq 1000 would mean 1000 ticks in 1 sec) */
void pit_init(uint32_t freq);

/* pit_read_freq: read the current count from the specified channel.
 * "port" is PIT_CHANNEL_N and "flag" is PIT_FLAG_CHANNEL_N */
uint16_t pit_read_count(enum pit_io_ports channel_port,
                        enum pit_cmd_flags channel_flag);

/* pit_dec: decrease the current tick count. Called from the PIT interrupt, on:
 * src/kernel/idt_asm.asm */
void pit_dec(void);

/* pit_set_ticks: sets the current PIT tick count. Called from the sleep functions
 * for setting the ammount of time we want to wait (src/libk/time.c) */
void pit_set_ticks(uint64_t num);

/* pit_get_ticks: returns the current PIT tick count. Called from the sleep functions
 * for checking if the ticks we set have passed or not (reached 0) */
uint64_t pit_get_ticks(void);

#endif /* _KERNEL_PIT_H */

