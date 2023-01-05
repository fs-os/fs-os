
#ifndef _KERNEL_COLOR_H
#define _KERNEL_COLOR_H

#include <stdint.h>

typedef struct {
    uint32_t fg;
    uint32_t bg;
} color_pair;

static inline uint32_t rgb2col(uint8_t r, uint8_t g, uint8_t b) {
    return (r << 16 | g << 8 | b);
}

static inline void col2rgb(uint8_t* r, uint8_t* g, uint8_t* b, uint32_t col) {
    *r = (col >> 16) & 0xff;
    *g = (col >> 8) & 0xff;
    *b = col & 0xff;
}

#endif /* _KERNEL_COLOR_H */

