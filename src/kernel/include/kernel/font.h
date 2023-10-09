
#ifndef KERNEL_FONT_H_
#define KERNEL_FONT_H_ 1

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t w;     /**< @brief Width */
    uint8_t h;     /**< @brief Height */
    char* name;    /**< @brief Font name */
    uint8_t* font; /**< @brief Bitmap ptr. Size must be (256 * w) */
} Font;

extern Font main_font;

/**
 * @brief Gets the bit at pos \p y, \p x of the char \p c from the Font \p font.
 * @details Would be the same as `font->font[c][y][x]` if it was a 3d array.
 * For more info see: src/kernel/include/fonts/main_font.h
 * @param font Pointer to the current Font struct.
 * @param c Current char containing the bit.
 * @param y Y position in px inside the character that we want to get.
 * @param x X position in px inside the character that we want to get.
 * @return True if the specified font bit should be displayed.
 */
static inline bool get_font_bit(Font* font, uint8_t c, uint8_t y, uint8_t x) {
    if (y > font->h)
        y = font->h;
    if (x > font->w)
        x = font->w;

    return font->font[c * font->h + y] & (0x80 >> x);
}

#endif /* KERNEL_FONT_H_ */
