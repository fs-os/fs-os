
#ifndef KERNEL_PIT_H_
#define KERNEL_PIT_H_ 1

#include <stdint.h>

/**
 * @def PIT_BASE_FREQ
 * @brief How many times the PIT oscillates in 1 sec by default (hz)
 */
#define PIT_BASE_FREQ 1193182

/**
 * @def PIT_INTERVAL_TO_FREQ
 * @brief Frequency in hz for generating the PIT interrupts.
 */
#define PIT_INTERVAL_TO_FREQ(freq) (PIT_BASE_FREQ / freq)

/**
 * @enum pit_io_ports
 * @brief I/O ports for the PIT (Programmable Interrupt Timer)
 */
enum pit_io_ports {
    PIT_CHANNEL_0   = 0x40, /**< @brief time.h functions (sleep, etc.) */
    PIT_CHANNEL_1   = 0x41, /**< @brief unused */
    PIT_CHANNEL_2   = 0x42, /**< @brief pc speaker */
    PIT_CHANNEL_CMD = 0x43, /**< @brief configuring frequencies */
};

/**
 * @enum pit_cmd_flags
 * @brief The PIT flags for the cmd cannel.
 */
enum pit_cmd_flags {
    PIT_FLAG_CHANNEL_0 = 0x00,
    PIT_FLAG_CHANNEL_1 = 0x40,
    PIT_FLAG_CHANNEL_2 = 0x80,
    PIT_FLAG_CHANNEL_3 = 0xC0,

    PIT_FLAG_ACCESS_COUNT = 0x00,
    PIT_FLAG_ACCESS_LO    = 0x10,
    PIT_FLAG_ACCESS_HI    = 0x20,
    PIT_FLAG_ACCESS_LOHI  = 0x30,

    PIT_FLAG_MODE_INT       = 0x00,
    PIT_FLAG_MODE_HW_RET    = 0x02,
    PIT_FLAG_MODE_SQUARE    = 0x04,
    PIT_FLAG_MODE_RATE      = 0x06,
    PIT_FLAG_MODE_SW_STROBE = 0x08,
    PIT_FLAG_MODE_HW_STROBE = 0x0A,
    PIT_FLAG_MODE_RATE_2    = 0x0C, /**< @brief Same as rate */
    PIT_FLAG_MODE_SQUARE_2  = 0x0E, /**< @brief Same as square */

    PIT_FLAG_BINARY_OFF = 0x0,
    PIT_FLAG_BINARY_BCD = 0x1,
};

/**
 * @brief Initialize the programmable interval timer with the specified
 * frequency in hz.
 * @param[in] freq Frequency in hz.
 */
void pit_init(uint32_t freq);

/**
 * @brief Read the current count from the specified channel.
 * @param[in] channel_port The port of the channel (PIT_CHANNEL_N)
 * @param[in] channel_flag The flag of the channel (PIT_FLAG_CHANNEL_N)
 * @return Current count from the specified channel.
 */
uint16_t pit_read_count(enum pit_io_ports channel_port,
                        enum pit_cmd_flags channel_flag);

/**
 * @brief Decrease the current tick count.
 * @details Currently unused.
 */
void pit_dec(void);

/**
 * @brief increase the current tick count.
 * @details Called from the PIT interrupt, on: src/kernel/idt.asm
 */
void pit_inc(void);

/**
 * @brief Sets the current PIT tick count.
 * @details Currently unused.
 * @param[in] num New tick count.
 */
void pit_set_ticks(uint64_t num);

/**
 * @brief Returns the current PIT tick count.
 * @details Called from the sleep functions for checking if the ticks we set
 * have passed or not (by comparing current ticks with starting ticks)
 * @return Current PIT tick count since boot.
 */
uint64_t pit_get_ticks(void);

#endif /* KERNEL_PIT_H_ */
