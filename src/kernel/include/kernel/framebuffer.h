
#ifndef _KERNEL_FRAMEBUFFER_H
#define _KERNEL_FRAMEBUFFER_H

#include <stdint.h>

#define FB_CONSOLE_BACKGROUND 0x111111

/*
 * See bottom of:
 *   https://www.gnu.org/software/grub/manual/multiboot/html_node/Boot-information-format.html
 */
enum fb_types {
    FB_TYPE_INDEXED = 0,
    FB_TYPE_RGB     = 1,
    FB_TYPE_EGA     = 2,
};

/* fb_init: initialize global framebuffer variables, clear the framebuffer and call
   vga_to_fb_arr. */
void fb_init(uint32_t* fb, uint32_t pitch, uint32_t w, uint32_t h, uint32_t bpp);

/* vga_to_fb_arr: move the VGA entry array to the new fb_char array */
void vga_to_fb_arr();

/* fb_setpx_col: Set the pixel at (y, x) of the global framebuffer to "col" */
void fb_setpx_col(uint32_t y, uint32_t x, uint32_t col);

/* fb_setpx_col: Set the pixel at (y, x) of the global framebuffer to the converted
 * color from "r", "g" and "b" */
void fb_setpx(uint32_t y, uint32_t x, uint8_t r, uint8_t g, uint8_t b);

#endif /* _KERNEL_FRAMEBUFFER_H */
