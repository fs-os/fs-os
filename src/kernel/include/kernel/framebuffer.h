
#ifndef _KERNEL_FRAMEBUFFER_H
#define _KERNEL_FRAMEBUFFER_H

#include <stdint.h>
#include <kernel/color.h>

/**
 * @brief Framebuffer types returned by the bootloader
 *
 * See bottom of:
 *   https://www.gnu.org/software/grub/manual/multiboot/html_node/Boot-information-format.html
 */
enum fb_types {
    FB_TYPE_INDEXED = 0,
    FB_TYPE_RGB     = 1, /**< @brief Requested */
    FB_TYPE_EGA     = 2,
};

/**
 * @brief Initialize global framebuffer variables and clear the framebuffer
 *
 * See Multiboot struct for more info
 *
 * @param fb Framebuffer address returned by the bootloader
 * @param pitch Framebuffer pitch
 * @param w Framebuffer width in px
 * @param h Framebuffer height in px
 * @param bpp Framebuffer bits per pixel
 */
void fb_init(uint32_t* fb, uint32_t pitch, uint32_t w, uint32_t h,
             uint32_t bpp);

/**
 * @brief Get the framebuffer ptr.
 *
 * Used by operations that need high performance like the fbc. Use with caution.
 *
 * @return Framebuffer address
 */
uint32_t* fb_get_ptr(void);

/**
 * @brief Get the framebuffer width in px
 * @return Framebuffer width in px
 */
uint32_t fb_get_width(void);

/**
 * @brief Get the framebuffer height in px
 * @return Framebuffer height in px
 */
uint32_t fb_get_height(void);

/**
 * @brief Set the pixel at \p y, \p x of the global framebuffer to \p col
 * @param y, x Position in px of the framebuffer
 * @param col New 32 bit color for that px
 */
void fb_setpx_col(uint32_t y, uint32_t x, uint32_t col);

/**
 * @brief RGB wrapper for fb_setpx_col()
 * @param y, x Position in px of the framebuffer
 * @param r, g, b Color in RGB format
 */
static inline void fb_setpx(uint32_t y, uint32_t x, uint8_t r, uint8_t g,
                            uint8_t b) {
    fb_setpx_col(y, x, rgb2col(r, g, b));
}

/**
 * @brief Fill \p h and \p w starting from \p y and \p x with color \p col
 * @param y, x Position in px of the framebuffer
 * @param h, w Height and width of the rectangle
 * @param col New 32 bit color for the rectangle
 */
void fb_drawrect_col(uint32_t y, uint32_t x, uint32_t h, uint32_t w,
                     uint32_t col);

/**
 * @brief RGB wrapper for fb_drawrect_col()
 * @param y, x Position in px of the framebuffer
 * @param h, w Height and width of the rectangle
 * @param r, g, b Color in RGB format
 */
static inline void fb_drawrect(uint32_t y, uint32_t x, uint32_t h, uint32_t w,
                               uint8_t r, uint8_t g, uint8_t b) {
    fb_drawrect_col(y, x, h, w, rgb2col(r, g, b));
}

/**
 * @brief Same as fb_drawrect_col() but less secure (no bounds check).
 *
 * Used when we know for sure we can control the parameters.
 *
 * @param y, x Position in px of the framebuffer
 * @param h, w Height and width of the rectangle
 * @param col New 32 bit color for the rectangle
 */
void fb_drawrect_fast(uint32_t y, uint32_t x, uint32_t h, uint32_t w,
                      uint32_t col);

#endif /* _KERNEL_FRAMEBUFFER_H */
