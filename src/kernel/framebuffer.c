
#include <kernel/framebuffer.h>

/* Framebuffer globals */
static uint32_t* g_fb; /* We will save the pointer, not a copy of the framebuffer */
static uint32_t g_pitch;
static uint32_t g_width;
static uint32_t g_height;
static uint32_t g_bpp;

static inline uint32_t rgb2col(uint8_t r, uint8_t g, uint8_t b) {
    return (r << 16 | g << 8 | b);
}

/* fb_init: initialize global framebuffer variables and clear the framebuffer */
void fb_init(uint32_t* fb, uint32_t pitch, uint32_t w, uint32_t h, uint32_t bpp) {
    /* Set globals to the parameter values we received from main (multiboot info) */
    g_fb     = fb;
    g_pitch  = pitch;
    g_width  = w;
    g_height = h;
    g_bpp    = bpp;

    for (uint32_t y = 0; y < h; y++) {
        for (uint32_t x = 0; x < w; x++) {
            g_fb[y * w + x] = 0x000000;
        }
    }
}

/* fb_setpx_col: set the pixel at (y, x) of the global framebuffer to "col" */
void fb_setpx_col(uint32_t y, uint32_t x, uint32_t col) {
    g_fb[y * g_width + x] = col;
}

/* fb_setpx: set the pixel at (y, x) of the global framebuffer to the converted
 * color from "r", "g" and "b".
 * TODO: Move wrappers to header as static inline? */
void fb_setpx(uint32_t y, uint32_t x, uint8_t r, uint8_t g, uint8_t b) {
    fb_setpx_col(y, x, rgb2col(r, g, b));
}

/* fb_drawrect_col: fill "h" and "w" starting from "y" and "x" with color "col" */
void fb_drawrect_col(uint32_t y, uint32_t x, uint32_t h, uint32_t w, uint32_t col) {
    for (; y < h; y++)
        for (; x < w; x++)
            g_fb[y * g_width + x] = col;
}

/* fb_drawrect: RGB wrapper for fb_drawrect_col */
void fb_drawrect(uint32_t y, uint32_t x, uint32_t h, uint32_t w, uint8_t r,
                 uint8_t g, uint8_t b) {
    fb_drawrect_col(y, x, h, w, rgb2col(r, g, b));
}

