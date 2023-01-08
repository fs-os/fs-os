
#include <stdint.h>
#include <kernel/io.h>
#include <kernel/datetime.h>

#define CENTURY 21

/* For accessing a register, we will need to write the register number we want to
 * "cmos_address" and then write or read from "cmos_data". For more info see:
 *   https://wiki.osdev.org/CMOS#Accessing_CMOS_Registers */
enum cmos_addrs {
    cmos_address = 0x70,
    cmos_data    = 0x71,
};

enum rtc_regs {
    reg_second  = 0x0,
    reg_minute  = 0x2,
    reg_hour    = 0x4,
    reg_day     = 0x7,
    reg_month   = 0x8,
    reg_year    = 0x9,
    reg_century = 0x32, /* unused */
    reg_a       = 0x0A,
    reg_b       = 0x0B,
};

/* is_updating: check if the "Update in progress" flag (bit 7 of register 0x0A) is
 * enabled */
static uint8_t is_updating(void) {
    io_outb(cmos_address, reg_a);
    return (io_inb(cmos_data) & 0x80);
}

/* get_reg: get the value from a register */
static uint8_t get_reg(enum rtc_regs reg) {
    /* Wait until we are not updating */
    while (is_updating())
        ;

    io_outb(cmos_address, reg);
    return io_inb(cmos_data);
}

/* rtc_get_data: get data (seconds, minutes, ...) from the specified register of the
 * Real Time Clock */
uint8_t rtc_get_data(enum rtc_regs reg) {
    uint8_t data = 0, last_data = 0;

    /* Loop to validate the value. Make sure we get the same values twice in a row */
    do {
        last_data = data;
        data      = get_reg(reg);
    } while (data != last_data);

    /* Check reg 0x0B to see if we need to convert to binary */
    if (!(get_reg(reg_b) & 0x04))
        data = (data & 0x0F) + ((data / 16) * 10);

    return data;
}

/* rtc_get_time: get a filled "Time" struct with the current hour, minute, and second
 * from the RTC */
Time rtc_get_time(void) {
    return (Time){
        rtc_get_data(reg_hour),
        rtc_get_data(reg_minute),
        rtc_get_data(reg_second),
    };
}

/* rtc_get_date: get a filled "Date" struct with the current day, month, and year
 * (century empty for now) from the RTC */
Date rtc_get_date(void) {
    /* TODO: Century from acpi */
    return (Date){
        rtc_get_data(reg_day),
        rtc_get_data(reg_month),
        rtc_get_data(reg_year),
        CENTURY,
    };
}

/* rtc_get_datetime: get a filled "DateTime" struct with the current Date from
 * rtc_get_date and the current Time from rtc_get_time */
DateTime rtc_get_datetime(void) {
    return (DateTime){
        rtc_get_date(),
        rtc_get_time(),
    };
}

