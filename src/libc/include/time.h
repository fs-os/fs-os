
#ifndef _TIME_H
#define _TIME_H

#include <stdint.h>

/* time: return seconds since epoch time (1-1-1970) */
uint32_t time(void* tloc);

/* sleep: sleep "sec" seconds */
void sleep(uint32_t sec);

/* sleep_ms: sleep "ms" milliseconds */
void sleep_ms(uint64_t ms);

/* timer_start: starts the pit timer. Calling multiple times resets the count */
void timer_start(void);

/* timer_stop: returns the ms difference from the current tick since the tick we
 * started the timer (time in ms that passed since we called timer_start). */
uint64_t timer_stop(void);

#endif /* _TIME_H */

