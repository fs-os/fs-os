
#ifndef _KERNEL_FONT_H
#define _KERNEL_FONT_H

#include <stdint.h>

typedef struct {
    uint8_t w;     /* Width */
    uint8_t h;     /* Height */
    uint8_t* name; /* Font name */
    uint8_t* font; /* Bitmap ptr. Size must be (256 * w) */
} Font;

/* get_font_bit: Gets the bit at pos (y, x) of the char "c" from the Font
 * "font". Would be the same as font->font[c][y][x] if it was a 3d array. For
 * more info see: src/kernel/include/fonts/main_font.h */
static inline uint8_t get_font_bit(Font* font, uint8_t c, uint8_t y,
                                   uint8_t x) {
    if (y > font->h)
        y = font->h;
    if (x > font->w)
        x = font->w;

    return font->font[c * font->h + y] & (0x80 >> x);
}

#endif /* _KERNEL_FONT_H */

