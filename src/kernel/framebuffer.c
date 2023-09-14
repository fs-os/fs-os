
#include <kernel/framebuffer.h>

/* Framebuffer globals */
static volatile uint32_t* g_fb;
static uint32_t g_pitch;
static uint32_t g_width;
static uint32_t g_height;
static uint32_t g_bpp;

void fb_init(volatile uint32_t* fb, uint32_t pitch, uint32_t w, uint32_t h,
             uint32_t bpp) {
    /* Set globals to the parameter values we received from main (multiboot
     * info) */
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

volatile uint32_t* fb_get_ptr(void) {
    return g_fb;
}

uint32_t fb_get_width(void) {
    return g_width;
}

uint32_t fb_get_height(void) {
    return g_height;
}

void fb_setpx_col(uint32_t y, uint32_t x, uint32_t col) {
    if (y >= g_height || x >= g_width)
        return;

    g_fb[y * g_width + x] = col;
}

void fb_drawrect_col(uint32_t y, uint32_t x, uint32_t h, uint32_t w,
                     uint32_t col) {
    if (y >= g_height || x >= g_width)
        return;

    uint32_t final_y = y + h;
    uint32_t final_x = x + w;

    /* Don't draw outside of the screen */
    if (final_y >= g_height)
        final_y = g_height - 1;

    if (final_x >= g_width)
        final_x = g_width - 1;

    for (uint32_t cur_y = y; cur_y < final_y; cur_y++)
        for (uint32_t cur_x = x; cur_x < final_x; cur_x++)
            g_fb[cur_y * g_width + cur_x] = col;
}

void fb_drawrect_fast(uint32_t y, uint32_t x, uint32_t h, uint32_t w,
                      uint32_t col) {
    for (uint32_t cur_y = y; cur_y < y + h; cur_y++)
        for (uint32_t cur_x = x; cur_x < x + w; cur_x++)
            g_fb[cur_y * g_width + cur_x] = col;
}

void fb_drawtext(uint32_t y, uint32_t x, color_pair cols, Font* font,
                 const char* s) {
    const uint32_t fb_w = fb_get_width();

    /* Iterate chars of string */
    for (; *s != '\0'; s++, x += font->w) {
        /* Then iterate each pixel that forms the font char */
        for (uint8_t fy = 0; fy < font->h; fy++) {
            for (uint8_t fx = 0; fx < font->w; fx++) {
                /* Screen coordinates of current pixel offset  */
                const uint32_t final_y = y + fy;
                const uint32_t final_x = x + fx;

                /* Depending on the font, set background or foreground */
                g_fb[final_y * fb_w + final_x] =
                  get_font_bit(font, *s, fy, fx) ? cols.fg : cols.bg;
            }
        }
    }
}

void fb_drawimage(uint32_t y, uint32_t x, const GimpImage* img) {
    if (y >= g_height || x >= g_width)
        return;

    uint32_t final_y = y + img->h;
    uint32_t final_x = x + img->w;

    /* Adjust final_y to not draw outside of the screen. If we exceed the
     * height, we are not going to draw anything on the X coord. See loop
     * contents for more info. */
    if (final_y >= g_height)
        final_y = g_height - 1;

    const char* data = img->data;
    uint8_t rgb[3];

    for (uint32_t cur_y = y; cur_y < final_y; cur_y++) {
        for (uint32_t cur_x = x; cur_x < final_x; cur_x++) {
            /* Need to call even if out of bounds because  */
            GIMP_GET_PIXEL(data, rgb);

            /* Unlike with final_y, we need to check here instead of adjusting
             * final_x because the macro needs to be called even when not
             * drawing. */
            if (cur_x >= g_width)
                continue;

            g_fb[cur_y * g_width + cur_x] = rgb2col(rgb[0], rgb[1], rgb[2]);
        }
    }
}
