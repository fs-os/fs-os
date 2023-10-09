
#ifndef TIME_H_
#define TIME_H_ 1

#include <stdint.h>

/**
 * @brief Return seconds since epoch time (1-1-1970)
 * @param[in] tloc Unused.
 * @return Seconds since epoch time.
 */
uint32_t time(void* tloc);

/**
 * @brief Sleep the specified amount of seconds.
 * @param[in] sec Seconds to sleep.
 */
void sleep(uint32_t sec);

/**
 * @brief Sleep the specified amount of milliseconds.
 * @param[in] ms Milliseconds to sleep.
 */
void sleep_ms(uint64_t ms);

/**
 * @brief Start the PIT timer.
 * @details Calling multiple times resets the count.
 */
void timer_start(void);

/**
 * @brief Get the ticks (ms) since we called timer_start()
 * @details Each tick should be 1 ms, see pit_init() call from kernel.c
 * @return Ticks since we called timer_start()
 */
uint64_t timer_stop(void);

#endif /* TIME_H_ */
