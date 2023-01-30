
#include <stdlib.h>
#include <kernel/color.h>
#include <kernel/vga.h> /* VGA_CONSOLE_ADDR */
#include <kernel/framebuffer.h>
#include <kernel/framebuffer_console.h>

#define DEFAULT_FG COLOR_WHITE
#define DEFAULT_BG COLOR_BLACK

/* Converts a char position in the fbc to a pixel position (top left corner) */
#define CHAR_Y_TO_PX(y) (g_y + (y * g_font->h * g_font->s))
#define CHAR_X_TO_PX(x) (g_x + (x * g_font->w * g_font->s))

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

/* Current color we are using when printing. Foreground and background */
static color_pair cur_cols;

/* Current position on the console */
static uint32_t cur_y, cur_x;

/* Bool that will be set to 1 if we know we are going to shift the console. See
 * framebuffer console wiki page */
static uint8_t should_shift;

/* ------------------------------------------------------------------------------- */

/* fbc_refresh_entry: refreshes the pixels on the screen corresponding to g_fbc's
 * entry at (cy, cx) */
static inline void fbc_refresh_entry(uint32_t cy, uint32_t cx) {
    /* Get the current fbc_entry */
    const fbc_entry cur_entry = g_fbc[cy * g_cw + cx];
    uint32_t* const fb_ptr    = fb_get_ptr();
    const uint32_t fb_w       = fb_get_width();

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
            fb_ptr[final_y * fb_w + final_x] =
              (get_font_bit(g_font, cur_entry.c, fy, fx)) ? cur_entry.fg
                                                          : cur_entry.bg;
        }
    }
}

/* ------------------------------------------------------------------------------- */

/* fbc_init: initialize the framebuffer console. First 4 parameters are position and
 * size in pixels of the console and the last one is the font. The font ptr is stored
 * and used to get the height and width of each char. */
void fbc_init(uint32_t y, uint32_t x, uint32_t h, uint32_t w, Font* font) {
    g_y = y;
    g_x = x;
    g_h = h;
    g_w = w;

    /* We get the font size but we save the console char dimensions */
    g_ch   = h / (font->h * font->s);
    g_cw   = w / (font->w * font->s);
    g_font = font;

    cur_cols.fg = DEFAULT_FG;
    cur_cols.bg = DEFAULT_BG;

    cur_y = 0;
    cur_x = 0;

    should_shift = 0;

    /* Allocate the number of fbc_entry's. Rows and cols of the console */
    g_fbc = malloc(g_ch * g_cw * sizeof(fbc_entry));

    fbc_clear();
    fbc_refresh();
}

/* fbc_clear: clears the framebuffer console and moves cursor to the first char */
void fbc_clear(void) {
    cur_x = 0;
    cur_y = 0;

    for (uint32_t cy = 0; cy < g_ch; cy++) {
        /* First entry is newline, rest spaces. We dont need to call
         * fbc_refresh_entry because we know the whole line is empty */
        g_fbc[cy * g_cw + 0] = (fbc_entry){
            '\n',
            DEFAULT_FG,
            DEFAULT_BG,
        };

        for (uint32_t cx = 1; cx < g_cw; cx++) {
            g_fbc[cy * g_cw + cx] = (fbc_entry){
                '\0',
                DEFAULT_FG,
                DEFAULT_BG,
            };
        }
    }
}

/* fbc_write: prints "len" of "s" to the framebuffer console using fbc_putchar */
void fbc_write(const char* s, size_t len) {
    while (len-- > 0)
        fbc_putchar(*s++);
}

/* fbc_sprint: prints zero-terminated string to the framebuffer console using
 * fbc_putchar */
void fbc_sprint(const char* s) {
    while (*s > '\0')
        fbc_putchar(*s++);
}

/* fbc_pace_str: place "str" with current colors at positions (y, x) of the
 * framebuffer console array. Warning: Overwrites! */
void fbc_place_str(uint32_t y, uint32_t x, const char* str) {
    while (*str != '\0' && y < g_ch && x < g_cw) {
        /* See fbc_putchar comment */
        if (should_shift) {
            fbc_shift_rows(1);
            should_shift = 0;
        }

        switch (*str) {
            case '\n':
                /* Save newline char (don't display anything) */
                g_fbc[y * g_cw + x] = (fbc_entry){
                    '\n',
                    DEFAULT_FG,
                    DEFAULT_BG,
                };

                /* If we have rows left on the terminal, go down, if we are on the
                 * last one, store that we need to shift 1 up, but stay on that last
                 * row. */
                if (y + 1 < g_ch)
                    y++;
                else
                    should_shift = 1;

                x = 0;

                str++;
                continue;
            case '\t':
                /* For having TABSIZE-aligned tabs */
                const int tabs_needed = FBC_TABSIZE - (x % FBC_TABSIZE);
                for (int i = 0; i < tabs_needed; i++)
                    fbc_putchar(' ');

                continue;
            case '\b':
                if (x > 0) {
                    x--;
                } else if (y > 0) {
                    y--;
                    x = g_cw - 1;

                    while (g_fbc[y * g_cw + x].c == '\0')
                        x--;
                }

                /* Clear the char we just deleted */
                g_fbc[y * g_cw + x] = (fbc_entry){
                    ' ',
                    DEFAULT_FG,
                    DEFAULT_BG,
                };

                /* Draw the pixels on the screen */
                fbc_refresh_entry(y, x);

                continue;
            case '\r':
                /* Fill from initial string pos to the cursor pos */
                const uint32_t fill_y = CHAR_Y_TO_PX(y);
                const uint32_t fill_x = CHAR_X_TO_PX(x);
                const uint32_t fill_h = g_font->h * g_font->s;
                const uint32_t fill_w = CHAR_X_TO_PX(x) - fill_x;
                fb_drawrect_fast(fill_y, fill_x, fill_h, fill_w, cur_cols.bg);

                cur_x = 0;
                return;
            default:
                break;
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
                should_shift = 1;
        }
    }
}

/* fbc_putchar: prints "c" to the framebuffer console */
void fbc_putchar(char c) {
    /* First of all, check if we need to shift the array. We need this kind of
     * "queue" system so the array doesn't inmediately shift when a line ends with
     * '\n', for example */
    if (should_shift) {
        fbc_shift_rows(1);
        should_shift = 0;
    }

    /* Check for special chars like newlines, tabs, backspaces, etc. */
    switch (c) {
        case '\n':
            /* Save newline char (don't display anything) */
            g_fbc[cur_y * g_cw + cur_x] = (fbc_entry){
                '\n',
                DEFAULT_FG,
                DEFAULT_BG,
            };

            /* If we have rows left on the terminal, go down, if we are on the last
             * one, store that we need to shift 1 up, but stay on that last row.
             * See comment on top of this function */
            if (cur_y + 1 < g_ch)
                cur_y++;
            else
                should_shift = 1;

            cur_x = 0;

            return;
        case '\t':
            /* For having TABSIZE-aligned tabs */
            const int tabs_needed = FBC_TABSIZE - (cur_x % FBC_TABSIZE);
            for (int i = 0; i < tabs_needed; i++)
                fbc_putchar(' ');

            return;
        case '\b':
            if (cur_x > 0) {
                cur_x--;
            } else if (cur_y > 0) {
                cur_y--;
                cur_x = g_cw - 1;

                while (g_fbc[cur_y * g_cw + cur_x].c == '\0')
                    cur_x--;
            }

            /* Clear the char we just deleted */
            g_fbc[cur_y * g_cw + cur_x] = (fbc_entry){
                ' ',
                DEFAULT_FG,
                DEFAULT_BG,
            };

            /* Draw the pixels on the screen */
            fbc_refresh_entry(cur_y, cur_x);

            return;
        case '\r':
            /* Fill from start of the line to the cursor pos */
            const uint32_t fill_y = CHAR_Y_TO_PX(cur_y);
            const uint32_t fill_x = CHAR_X_TO_PX(0);
            const uint32_t fill_h = g_font->h * g_font->s;
            const uint32_t fill_w = CHAR_X_TO_PX(cur_x) - fill_x;
            fb_drawrect_fast(fill_y, fill_x, fill_h, fill_w, cur_cols.bg);

            cur_x = 0;
            return;
        default:
            break;
    }

    g_fbc[cur_y * g_cw + cur_x] = (fbc_entry){
        c,
        cur_cols.fg,
        cur_cols.bg,
    };

    /* Draw the pixels on the screen */
    fbc_refresh_entry(cur_y, cur_x);

    /* If we reach the end of the line, reset x and increase y */
    if (++cur_x >= g_cw) {
        cur_x = 0;

        /* See comment on newline conditional */
        if (cur_y + 1 < g_ch)
            cur_y++;
        else
            should_shift = 1;
    }
}

/* fbc_refresh: updates each pixel of the framebuffer with the real one in g_fbc.
 * Calling this function everytime we update g_fbc would be slow. Insead call this
 * function on specific situations and we refresh the entries we need when updating
 * g_fbc (e.g. when calling fbc_putchar) */
void fbc_refresh(void) {
    /* First iterate each char of the framebuffer console */
    for (uint32_t cy = 0; cy < g_ch; cy++)
        for (uint32_t cx = 0; cx < g_cw; cx++)
            fbc_refresh_entry(cy, cx);
}

/* fbc_shift_rows: scrolls the framebuffer terminal "n" rows (fbc_entry's) */
/* TODO: Still not very fast. Optimize */
void fbc_shift_rows(uint8_t n) {
    /* Used to count the position of the last valid char in the line */
    uint32_t char_count = 0;

    /* Shift n rows. We go to the newline instead of always g_cw because that will be
     * the last valid char we care about. We can fill the rest faster with
     * fb_drawrect_fast */
    for (uint32_t y = 0; y < g_ch - n; y++) {
        /* Update valid entries until we encounter a null byte. '\0' denotes the end
         * of the valid line. */
        for (uint32_t x = 0; x < g_cw; x++) {
            g_fbc[y * g_cw + x] = g_fbc[(y + n) * g_cw + x];

            /* We need to check after the assignment and not in the for because we
             * want to also copy the null byte to keep where the line ends */
            if (g_fbc[(y + n) * g_cw + x].c == '\0')
                break;

            fbc_refresh_entry(y, x);

            char_count++;
        }

        /* Fill from last valid to the end of the line */
        const uint32_t fill_y = CHAR_Y_TO_PX(y);
        const uint32_t fill_x = CHAR_X_TO_PX(char_count);
        const uint32_t fill_h = g_font->h * g_font->s;
        const uint32_t fill_w = g_w + g_x - fill_x;
        fb_drawrect_fast(fill_y, fill_x, fill_h, fill_w, DEFAULT_BG);

        char_count = 0;
    }

    /* Clear last n rows with clean entries. Only change the ones that were full */
    for (uint32_t y = g_ch - n; y < g_ch; y++) {
        /* First entry is newline, rest spaces. We dont need to call
         * fbc_refresh_entry because we know the whole line is empty */
        g_fbc[y * g_cw + 0] = (fbc_entry){
            '\n',
            DEFAULT_FG,
            DEFAULT_BG,
        };

        for (uint32_t x = 1; x < g_cw; x++) {
            g_fbc[y * g_cw + x] = (fbc_entry){
                '\0',
                DEFAULT_FG,
                DEFAULT_BG,
            };
        }
    }

    /* Fill last empty lines with background color */
    const uint32_t fill_y = CHAR_Y_TO_PX(g_ch - n);
    const uint32_t fill_x = CHAR_X_TO_PX(0);
    const uint32_t fill_h = n * g_font->h * g_font->s;
    const uint32_t fill_w = g_w;
    fb_drawrect_fast(fill_y, fill_x, fill_h, fill_w, DEFAULT_BG);
}

/* ------------------------------------------------------------------------------- */

/* fbc_getcols: writes the current colors of the terminal to "fg" and "bg" */
void fbc_getcols(uint32_t* fg, uint32_t* bg) {
    *fg = cur_cols.fg;
    *bg = cur_cols.bg;
}

/* fbc_setcol: sets the current foreground and background colors */
void fbc_setcol(uint32_t fg, uint32_t bg) {
    cur_cols.fg = fg;
    cur_cols.bg = bg;
}

/* fbc_setfore: change the current foreground color */
void fbc_setfore(uint32_t fg) {
    cur_cols.fg = fg;
}

/* fbc_setback: change the current background color */
void fbc_setback(uint32_t bg) {
    cur_cols.bg = bg;
}

/* fbc_setcol_rgb: sets the current foreground and background colors in rgb format */
void fbc_setcol_rgb(uint8_t fore_r, uint8_t fore_g, uint8_t fore_b, uint8_t back_r,
                    uint8_t back_g, uint8_t back_b) {
    cur_cols.fg = rgb2col(fore_r, fore_g, fore_b);
    cur_cols.bg = rgb2col(back_r, back_g, back_b);
}

