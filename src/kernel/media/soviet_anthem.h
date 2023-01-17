
#include <kernel/pcspkr.h>

#define LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

Beep soviet_anthem[] = {
    { .freq = 392, .ms_len = 375 },  { .freq = 523, .ms_len = 750 },
    { .freq = 392, .ms_len = 463 },  { .freq = 440, .ms_len = 187 },
    { .freq = 494, .ms_len = 750 },  { .freq = 330, .ms_len = 375 },
    { .freq = 330, .ms_len = 375 },  { .freq = 440, .ms_len = 750 },
    { .freq = 392, .ms_len = 463 },  { .freq = 349, .ms_len = 187 },
    { .freq = 392, .ms_len = 750 },  { .freq = 262, .ms_len = 463 },
    { .freq = 262, .ms_len = 187 },  { .freq = 294, .ms_len = 750 },
    { .freq = 294, .ms_len = 463 },  { .freq = 330, .ms_len = 187 },
    { .freq = 349, .ms_len = 750 },  { .freq = 349, .ms_len = 463 },
    { .freq = 392, .ms_len = 187 },  { .freq = 440, .ms_len = 750 },
    { .freq = 494, .ms_len = 375 },  { .freq = 523, .ms_len = 375 },
    { .freq = 587, .ms_len = 1125 },
};

