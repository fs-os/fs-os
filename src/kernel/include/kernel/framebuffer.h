
#ifndef _KERNEL_FRAMEBUFFER_H
#define _KERNEL_FRAMEBUFFER_H

#include <stdint.h>
#include <kernel/color.h>

/*
 * See bottom of:
 *   https://www.gnu.org/software/grub/manual/multiboot/html_node/Boot-information-format.html
 */
enum fb_types {
    FB_TYPE_INDEXED = 0,
    FB_TYPE_RGB     = 1,
    FB_TYPE_EGA     = 2,
};

/* fb_init: initialize global framebuffer variables and clear the framebuffer */
void fb_init(uint32_t* fb, uint32_t pitch, uint32_t w, uint32_t h, uint32_t bpp);

/* fb_get_ptr: get the framebuffer ptr. Used by operations that need high performance
 * like the fbc. Use with caution. */
uint32_t* fb_get_ptr(void);

/* fb_get_width: get the framebuffer width in px */
uint32_t fb_get_width(void);

/* fb_get_height: get the framebuffer height in px */
uint32_t fb_get_height(void);

/* fb_setpx_col: set the pixel at (y, x) of the global framebuffer to "col" */
void fb_setpx_col(uint32_t y, uint32_t x, uint32_t col);

/* fb_setpx: RGB wrapper for fb_setpx_col */
static inline void fb_setpx(uint32_t y, uint32_t x, uint8_t r, uint8_t g,
                            uint8_t b) {
    fb_setpx_col(y, x, rgb2col(r, g, b));
}

/* fb_drawrect_col: fill "h" and "w" starting from "y" and "x" with color "col" */
void fb_drawrect_col(uint32_t y, uint32_t x, uint32_t h, uint32_t w, uint32_t col);

/* fb_drawrect: RGB wrapper for fb_drawrect_col */
static inline void fb_drawrect(uint32_t y, uint32_t x, uint32_t h, uint32_t w,
                               uint8_t r, uint8_t g, uint8_t b) {
    fb_drawrect_col(y, x, h, w, rgb2col(r, g, b));
}

/* fb_drawrect_fast: same as fb_drawrect_col but less secure. Used when we know for
 * sure we can control the parameters. */
void fb_drawrect_fast(uint32_t y, uint32_t x, uint32_t h, uint32_t w, uint32_t col);

#endif /* _KERNEL_FRAMEBUFFER_H */
