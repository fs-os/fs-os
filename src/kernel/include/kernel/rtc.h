
#ifndef KERNEL_RTC_H_
#define KERNEL_RTC_H_ 1

#include <stdint.h>
#include <kernel/datetime.h>

/**
 * @brief Get data (seconds, minutes, ...) from the specified register of the
 * Real Time Clock
 * @param[in] reg Register from the rtc_regs struct.
 * @return The data from that register.
 */
uint8_t rtc_get_data(enum rtc_regs reg);

/**
 * @brief Get a filled Time struct with the current hour, minute, and second
 * from the RTC
 * @return Time struct with the current time.
 */
Time rtc_get_time(void);

/* rtc_get_date:  */

/**
 * @brief Get a filled Date struct with the current day, month, and year from
 * the RTC.
 * @details Century empty for now.
 * @return Date struct with the current date.
 */
Date rtc_get_date(void);

/**
 * @brief Get a filled DateTime struct with the current Date from rtc_get_date()
 * and the current Time from rtc_get_time().
 * @return DateTime struct with the current date and time.
 */
DateTime rtc_get_datetime(void);

#endif /* KERNEL_RTC_H_ */
