
/**
 * @brief Real Time Clock.
 *
 * See: https://wiki.osdev.org/RTC
 *
 * @file
 */

#include <stdint.h>
#include <kernel/io.h>
#include <kernel/datetime.h>

#define CENTURY 21

/**
 * @brief I/O ports for the cmos.
 *
 * For accessing a register, we will need to write the register number we want
 * to "cmos_address" and then write or read from "cmos_data".
 *
 * For more information see:
 *   https://wiki.osdev.org/CMOS#Accessing_CMOS_Registers
 */
enum cmos_addrs {
    cmos_address = 0x70,
    cmos_data    = 0x71,
};

/**
 * @brief Registers for the cmos_address port used to read data.
 */
enum rtc_regs {
    reg_second  = 0x0,
    reg_minute  = 0x2,
    reg_hour    = 0x4,
    reg_day     = 0x7,
    reg_month   = 0x8,
    reg_year    = 0x9,
    reg_century = 0x32, /**< @brief Unused */
    reg_a = 0x0A, /**< @brief For checking the "Update in progress" flag */
    reg_b = 0x0B, /**< @brief For checking if we should convert data to
                    binary */
};

/**
 * @brief Check if the "Update in progress" flag (bit 7 of register 0x0A)
 * is enabled.
 * @return 0 if not updating, >0 if updating.
 */
static uint8_t is_updating(void) {
    io_outb(cmos_address, reg_a);
    return (io_inb(cmos_data) & 0x80);
}

/**
 * @brief Get the value from a register
 * @param reg Register number from the rtc_regs enum
 * @return Value from that register
 */
static uint8_t get_reg(enum rtc_regs reg) {
    /* Wait until we are not updating */
    while (is_updating())
        ;

    io_outb(cmos_address, reg);
    return io_inb(cmos_data);
}

uint8_t rtc_get_data(enum rtc_regs reg) {
    uint8_t data = 0, last_data = 0;

    /* Loop to validate the value. Make sure we get the same values twice in a
     * row */
    do {
        last_data = data;
        data      = get_reg(reg);
    } while (data != last_data);

    /* Check reg 0x0B to see if we need to convert to binary */
    if (!(get_reg(reg_b) & 0x04))
        data = (data & 0x0F) + ((data / 16) * 10);

    return data;
}

Time rtc_get_time(void) {
    return (Time){
        rtc_get_data(reg_hour),
        rtc_get_data(reg_minute),
        rtc_get_data(reg_second),
    };
}

Date rtc_get_date(void) {
    /** @todo Century from acpi */
    return (Date){
        rtc_get_data(reg_day),
        rtc_get_data(reg_month),
        rtc_get_data(reg_year),
        CENTURY,
    };
}

DateTime rtc_get_datetime(void) {
    return (DateTime){
        rtc_get_date(),
        rtc_get_time(),
    };
}

