
#ifndef _TIME_H
#define _TIME_H

#include <stdint.h>

/* sleep: sleep "sec" seconds */
void sleep(uint32_t sec);

/* sleep_cs: sleep "ms" milliseconds */
void sleep_cs(uint64_t ms);

#endif /* _TIME_H */

