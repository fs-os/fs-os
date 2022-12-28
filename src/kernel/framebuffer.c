
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

/* fb_init: initialize global framebuffer variables, clear the framebuffer and call
   vga_to_fb_arr. */
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

    vga_to_fb_arr();
}

/*
 * TODO: Allocate a "fb_char" array for the framebuffer and copy the characters from
 * the VGA buffer to there. "fb_char" will be a struct containing the char itself and
 * the char color (fb and bg).
 * That way we can write to the VGA arr or the FB arr in stdio.h without changing the
 * function calls in kernel_main.
 */
void vga_to_fb_arr() {
}

