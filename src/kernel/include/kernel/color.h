
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

/* Color palette */
#ifndef COLOR_BLACK
#define COLOR_BLACK 0x000000
#endif

#ifndef COLOR_RED
#define COLOR_RED 0xff1c23
#endif

#ifndef COLOR_GREEN
#define COLOR_GREEN 0x23ff1c
#endif

#ifndef COLOR_YELLOW
#define COLOR_YELLOW 0xfff71c
#endif

#ifndef COLOR_BLUE
#define COLOR_BLUE 0x274dca
#endif

#ifndef COLOR_MAGENTA
#define COLOR_MAGENTA 0x7547b2 /* 0x7852a9 */
#endif

#ifndef COLOR_PURPLE
#define COLOR_PURPLE COLOR_MAGENTA
#endif

#ifndef COLOR_CYAN
#define COLOR_CYAN 0x1cfff7
#endif

#ifndef COLOR_WHITE
#define COLOR_WHITE 0xd9d9d9
#endif

/* Bold colors */
#ifndef COLOR_BLACK_B
#define COLOR_BLACK_B 0x3f3f3f
#endif

#ifndef COLOR_RED_B
#define COLOR_RED_B 0xf92672
#endif

#ifndef COLOR_GREEN_B
#define COLOR_GREEN_B 0xa6e22e
#endif

#ifndef COLOR_YELLOW_B
#define COLOR_YELLOW_B 0xcaa427
#endif

#ifndef COLOR_BLUE_B
#define COLOR_BLUE_B 0x1c23ff
#endif

#ifndef COLOR_MAGENTA_B
#define COLOR_MAGENTA_B 0xae81ff
#endif

#ifndef COLOR_PURPLE_B
#define COLOR_PURPLE_B COLOR_MAGENTA_B
#endif

#ifndef COLOR_CYAN_B
#define COLOR_CYAN_B 0x2aa198
#endif

#ifndef COLOR_WHITE_B
#define COLOR_WHITE_B 0xffffff
#endif

#endif /* _KERNEL_COLOR_H */

