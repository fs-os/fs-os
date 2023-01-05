
#ifndef _KERNEL_FRAMEBUFFER_CONSOLE_H
#define _KERNEL_FRAMEBUFFER_CONSOLE_H

#include <stdint.h>
#include <stddef.h>
#include <kernel/font.h>

/* Framebuffer console entry. Char and rgb fg and bg in uint32_t format */
typedef struct {
    uint8_t c;
    uint32_t fg;
    uint32_t bg;
} fbc_entry;

/* fbc_init: initialize the framebuffer console. First 4 parameters are position and
 * size in pixels of the console and the last one is the font. The font ptr is stored
 * and used to get the height and width of each char. */
void fbc_init(uint32_t y, uint32_t x, uint32_t px_h, uint32_t px_w, Font* font);

/* fbc_refresh: updates each pixel of the framebuffer with the real one in g_fbc.
 * Calling this function everytime we update g_fbc would be slow. Insead call this
 * function on specific situations and we refresh the framebuffer pixels we need when
 * updating g_fbc (e.g. when calling fbc_putchar) */
void fbc_refresh();

/* fbc_pace_str: place "str" with default colors at positions (y, x) of the
 * framebuffer console array. Warning: Overwrites! */
void fbc_place_str(size_t y, size_t x, const char* str);

/* fbc_shift_rows: scrolls the framebuffer terminal "n" rows (fbc_entry's) */
void fbc_shift_rows(uint8_t n);

#endif /* _KERNEL_FRAMEBUFFER_CONSOLE_H */
