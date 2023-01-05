
#include <stdlib.h>
#include <kernel/framebuffer.h>
#include <kernel/framebuffer_console.h>

/* TODO: Color palette */
#define DEFAULT_FG 0xEEEEEE
#define DEFAULT_BG 0x000000

/* Global framebuffer console. Allocated in fbc_init(). The framebuffer console array
 * won't be used for displaying (outside of fbc_refresh), but for keeping track of
 * where is each char on the console. */
static fbc_entry* g_fbc;

/* Global size in px of the framebuffer console */
static uint32_t g_y, g_x, g_h, g_w;

/* Global number of entry rows and cols that form the console (chars) */
static uint32_t g_ch, g_cw;

/* Global pointer to the font the framebufer console uses */
static Font* g_font;

/* fbc_init: initialize the framebuffer console. First 4 parameters are position and
 * size in pixels of the console and the last one is the font. The font ptr is stored
 * and used to get the height and width of each char. */
void fbc_init(uint32_t y, uint32_t x, uint32_t px_h, uint32_t px_w, Font* font) {
    g_y = y;
    g_x = x;
    g_h = px_h;
    g_w = px_w;
    /* We get the font size but we save the console char dimensions */
    g_ch   = px_h / (font->h * font->s);
    g_cw   = px_w / (font->w * font->s);
    g_font = font;

    /* Allocate the number of fbc_entry's. Rows and cols of the console */
    g_fbc = malloc(g_ch * g_cw * sizeof(fbc_entry));

    for (uint32_t cy = 0; cy < g_ch; cy++) {
        for (uint32_t cx = 0; cx < g_cw; cx++) {
            g_fbc[cy * g_cw + cx] = (fbc_entry){
                ' ',
                DEFAULT_FG,
                DEFAULT_BG,
            };
        }
    }

    fbc_refresh();
}

/* fbc_refresh: updates each pixel of the framebuffer with the real one in g_fbc.
 * Calling this function everytime we update g_fbc would be slow. Insead call this
 * function on specific situations and we refresh the framebuffer pixels we need when
 * updating g_fbc (e.g. when calling fbc_putchar) */
void fbc_refresh() {
    /* First iterate each char of the framebuffer console */
    for (uint32_t cy = 0; cy < g_ch; cy++) {
        for (uint32_t cx = 0; cx < g_cw; cx++) {
            /* Get the current fbc_entry */
            const fbc_entry cur_entry = g_fbc[cy * g_cw + cx];

            /* Then iterate each pixel that forms the font char */
            for (uint8_t fy = 0; fy < g_font->h; fy++) {
                for (uint8_t fx = 0; fx < g_font->w; fx++) {
                    /* Get real screen position. We add g_y and g_x because its the
                     * console px offset (position). We multiply the font position by
                     * the font scale. */
                    const uint32_t final_y =
                      g_y + ((cy * g_font->h * g_font->s) + (fy * g_font->s));
                    const uint32_t final_x =
                      g_x + ((cx * g_font->w * g_font->s) + (fx * g_font->s));

                    /* We check if the current bit of the current column is 1.
                     * Depending on that, set it to foreground or background. For
                     * more information see: src/kernel/include/kernel/font.h */
                    if (get_font_bit(g_font, cur_entry.c, fy, fx))
                        fb_drawrect_col(final_y, final_x, g_font->s, g_font->s,
                                        cur_entry.fg);
                    else
                        fb_drawrect_col(final_y, final_x, g_font->s, g_font->s,
                                        cur_entry.bg);
                }
            }
        }
    }
}

/* fbc_pace_str: place "str" with default colors at positions (y, x) of the
 * framebuffer console array. Warning: Overwrites! */
void fbc_place_str(uint32_t y, uint32_t x, const char* str) {
    while (*str != '\0' && y < g_ch && x < g_cw) {
        if (*str == '\n') {
            /* If we have rows left on the terminal, go down, if we are on the last
             * one, shift 1 up, but stay on that last row */
            if (y + 1 < g_ch)
                y++;
            else
                fbc_shift_rows(1);

            x = 0;

            str++;
            continue;
        }

        g_fbc[y * g_cw + x] = (fbc_entry){
            *str,
            DEFAULT_FG,
            DEFAULT_BG,
        };
        str++;

        /* If we reach the end of the line, reset x and increase y */
        if (++x >= g_cw) {
            x = 0;

            /* See comment on newline conditional */
            if (y + 1 < g_ch)
                y++;
            else
                fbc_shift_rows(1);
        }
    }
}

/* fbc_shift_rows: scrolls the framebuffer terminal "n" rows (fbc_entry's) */
void fbc_shift_rows(uint8_t n) {
    /* Shift n rows */
    for (uint32_t y = 0; y < g_ch - n; y++)
        for (uint32_t x = 0; x < g_cw; x++)
            g_fbc[y * g_cw + x] = g_fbc[(y + n) * g_ch + x];

    /* Clear last n rows with clean entries */
    for (uint32_t y = g_ch - n; y < g_ch; y++)
        for (uint32_t x = 0; x < g_cw; x++)
            g_fbc[y * g_cw + x] = (fbc_entry){
                ' ',
                DEFAULT_FG,
                DEFAULT_BG,
            };
}

void fbc_putchar() {

}

