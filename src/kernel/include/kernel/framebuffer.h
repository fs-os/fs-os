
#ifndef _KERNEL_FRAMEBUFFER_H
#define _KERNEL_FRAMEBUFFER_H

#include <stdint.h>

#define FB_CONSOLE_BACKGROUND

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

#endif /* _KERNEL_FRAMEBUFFER_H */
