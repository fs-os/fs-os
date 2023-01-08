
#ifndef _KERNEL_DATETIME_H
#define _KERNEL_DATETIME_H

typedef struct {
    uint8_t h; /* hour */
    uint8_t m; /* minute */
    uint8_t s; /* sec */
} Time;

typedef struct {
    uint8_t d;  /* day */
    uint8_t m;  /* month */
    uint16_t y; /* year */
    uint8_t c;  /* century */
} Date;

typedef struct {
    Date date;
    Time time;
} DateTime;

#endif /* _KERNEL_DATETIME_H */

