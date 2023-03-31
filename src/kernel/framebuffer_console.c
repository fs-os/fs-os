
#include <stdbool.h>
#include <stdlib.h>
#include <kernel/color.h>
#include <kernel/vga.h> /* VGA_CONSOLE_ADDR */
#include <kernel/framebuffer.h>
#include <kernel/framebuffer_console.h>

#define DEFAULT_FG COLOR_WHITE
#define DEFAULT_BG COLOR_BLACK

/* Converts a char position in the fbc to a pixel position (top left corner) */
#define CHAR_Y_TO_PX(fy) (ctx.y + ((fy)*ctx.font->h))
#define CHAR_X_TO_PX(fx) (ctx.x + ((fx)*ctx.font->w))

/*
 * Current context of the framebuffer console. For more information see:
 *   src/kernel/include/kernel/framebuffer_console.h
 * Properties:
 *   - fbc_entry* fbc
 *   - uint32_t y, x, h, w
 *   - uint32_t ch_h, ch_w
 *   - Font* font
 *   - color_pair cur_cols
 *   - uint32_t cur_y, cur_x
 */
static fbc_ctx ctx;

/* Bool that will be set to 1 if we know we are going to shift the console. See
 * framebuffer console wiki page */
static bool should_shift;

/* -------------------------------------------------------------------------- */

/* fbc_refresh_entry: refreshes the pixels on the screen corresponding to
 * ctx.fbc's entry at (cy, cx) */
static inline void fbc_refresh_entry(uint32_t cy, uint32_t cx) {
    /* Get the current fbc_entry */
    const fbc_entry cur_entry = ctx.fbc[cy * ctx.ch_w + cx];
    uint32_t* const fb_ptr    = fb_get_ptr();
    const uint32_t fb_w       = fb_get_width();

    /* Then iterate each pixel that forms the font char */
    for (uint8_t fy = 0; fy < ctx.font->h; fy++) {
        for (uint8_t fx = 0; fx < ctx.font->w; fx++) {
            /* Get real screen position from the char offset on the screen and
             * the pixel font offset on the char */
            const uint32_t final_y = CHAR_Y_TO_PX(cy) + fy;
            const uint32_t final_x = CHAR_X_TO_PX(cx) + fx;

            /* We check if the current bit of the current column is 1.
             * Depending on that, set it to foreground or background. For
             * more information see: src/kernel/include/kernel/font.h */
            fb_ptr[final_y * fb_w + final_x] =
              (get_font_bit(ctx.font, cur_entry.c, fy, fx)) ? cur_entry.fg
                                                            : cur_entry.bg;
        }
    }
}

/* -------------------------------------------------------------------------- */

/* fbc_init: initialize the framebuffer console. First 4 parameters are position
 * and size in pixels of the console and the last one is the font. The font ptr
 * is stored and used to get the height and width of each char. */
void fbc_init(uint32_t y, uint32_t x, uint32_t h, uint32_t w, Font* font) {
    ctx.y = y;
    ctx.x = x;
    ctx.h = h;
    ctx.w = w;

    /* We get the font size but we save the console char dimensions */
    ctx.ch_h = h / font->h;
    ctx.ch_w = w / font->w;
    ctx.font = font;

    ctx.cur_cols.fg = DEFAULT_FG;
    ctx.cur_cols.bg = DEFAULT_BG;

    ctx.cur_y = 0;
    ctx.cur_x = 0;

    should_shift = false;

    /* Allocate the number of fbc_entry's. Rows and cols of the console */
    ctx.fbc = malloc(ctx.ch_h * ctx.ch_w * sizeof(fbc_entry));

    fbc_clear();
    fbc_refresh();
}

/* fbc_change_ctx: switches to the specified framebuffer console context */
void fbc_change_ctx(fbc_ctx* new_ctx) {
    ctx = *new_ctx;
}

/* fbc_get_ctx: returns the current framebuffer console context */
fbc_ctx* fbc_get_ctx(void) {
    return &ctx;
}

/* fbc_clear: clears the framebuffer console and moves cursor to the first
 * char */
void fbc_clear(void) {
    ctx.cur_x = 0;
    ctx.cur_y = 0;

    for (uint32_t cy = 0; cy < ctx.ch_h; cy++) {
        /* First entry is newline, rest spaces. We dont need to call
         * fbc_refresh_entry because we know the whole line is empty */
        ctx.fbc[cy * ctx.ch_w + 0] = (fbc_entry){
            '\n',
            DEFAULT_FG,
            DEFAULT_BG,
        };

        for (uint32_t cx = 1; cx < ctx.ch_w; cx++) {
            ctx.fbc[cy * ctx.ch_w + cx] = (fbc_entry){
                '\0',
                DEFAULT_FG,
                DEFAULT_BG,
            };
        }
    }
}

/* fbc_write: prints "len" of "s" to the framebuffer console using
 * fbc_putchar */
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
    while (*str != '\0' && y < ctx.ch_h && x < ctx.ch_w) {
        /* See fbc_putchar comment */
        if (should_shift) {
            fbc_shift_rows(1);
            should_shift = false;
        }

        switch (*str) {
            case '\n':
                /* Save newline char (don't display anything) */
                ctx.fbc[y * ctx.ch_w + x] = (fbc_entry){
                    '\n',
                    DEFAULT_FG,
                    DEFAULT_BG,
                };

                /* If we have rows left on the terminal, go down, if we are on
                 * the last one, store that we need to shift 1 up, but stay on
                 * that last row. */
                if (y + 1 < ctx.ch_h)
                    y++;
                else
                    should_shift = true;

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
                    x = ctx.ch_w - 1;

                    while (ctx.fbc[y * ctx.ch_w + x].c == '\0')
                        x--;
                }

                /* Clear the char we just deleted */
                ctx.fbc[y * ctx.ch_w + x] = (fbc_entry){
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
                const uint32_t fill_h = ctx.font->h;
                const uint32_t fill_w = CHAR_X_TO_PX(x) - fill_x;
                fb_drawrect_fast(fill_y, fill_x, fill_h, fill_w,
                                 ctx.cur_cols.bg);

                for (uint32_t tmp_x = 0; tmp_x < x; tmp_x++) {
                    ctx.fbc[y * ctx.ch_w + tmp_x] = (fbc_entry){
                        ' ',
                        ctx.cur_cols.fg,
                        ctx.cur_cols.bg,
                    };
                }

                x = 0;
                return;
            default:
                break;
        }

        ctx.fbc[y * ctx.ch_w + x] = (fbc_entry){
            *str,
            DEFAULT_FG,
            DEFAULT_BG,
        };
        str++;

        /* If we reach the end of the line, reset x and increase y */
        if (++x >= ctx.ch_w) {
            x = 0;

            /* See comment on newline conditional */
            if (y + 1 < ctx.ch_h)
                y++;
            else
                should_shift = true;
        }
    }
}

/* fbc_putchar: prints "c" to the framebuffer console */
void fbc_putchar(char c) {
    /* First of all, check if we need to shift the array. We need this kind of
     * "queue" system so the array doesn't immediately shift when a line ends
     * with
     * '\n', for example */
    if (should_shift) {
        fbc_shift_rows(1);
        should_shift = false;
    }

    /* Check for special chars like newlines, tabs, backspaces, etc. */
    switch (c) {
        case '\n':
            /* Save newline char (don't display anything) */
            ctx.fbc[ctx.cur_y * ctx.ch_w + ctx.cur_x] = (fbc_entry){
                '\n',
                DEFAULT_FG,
                DEFAULT_BG,
            };

            /* If we have rows left on the terminal, go down, if we are on the
             * last one, store that we need to shift 1 up, but stay on that last
             * row. See comment on top of this function */
            if (ctx.cur_y + 1 < ctx.ch_h)
                ctx.cur_y++;
            else
                should_shift = true;

            ctx.cur_x = 0;

            return;
        case '\t':
            /* For having TABSIZE-aligned tabs */
            const int tabs_needed = FBC_TABSIZE - (ctx.cur_x % FBC_TABSIZE);
            for (int i = 0; i < tabs_needed; i++)
                fbc_putchar(' ');

            return;
        case '\b':
            if (ctx.cur_x > 0) {
                ctx.cur_x--;
            } else if (ctx.cur_y > 0) {
                ctx.cur_y--;
                ctx.cur_x = ctx.ch_w - 1;

                while (ctx.fbc[ctx.cur_y * ctx.ch_w + ctx.cur_x].c == '\0')
                    ctx.cur_x--;
            }

            /* Clear the char we just deleted */
            ctx.fbc[ctx.cur_y * ctx.ch_w + ctx.cur_x] = (fbc_entry){
                ' ',
                DEFAULT_FG,
                DEFAULT_BG,
            };

            /* Draw the pixels on the screen */
            fbc_refresh_entry(ctx.cur_y, ctx.cur_x);

            return;
        case '\r':
            /* Fill from start of the line to the cursor pos */
            const uint32_t fill_y = CHAR_Y_TO_PX(ctx.cur_y);
            const uint32_t fill_x = CHAR_X_TO_PX(0);
            const uint32_t fill_h = ctx.font->h;
            const uint32_t fill_w = CHAR_X_TO_PX(ctx.cur_x) - fill_x;
            fb_drawrect_fast(fill_y, fill_x, fill_h, fill_w, ctx.cur_cols.bg);

            for (uint32_t tmp_x = 0; tmp_x < ctx.cur_x; tmp_x++) {
                ctx.fbc[ctx.cur_y * ctx.ch_w + tmp_x] = (fbc_entry){
                    ' ',
                    ctx.cur_cols.fg,
                    ctx.cur_cols.bg,
                };
            }

            ctx.cur_x = 0;
            return;
        default:
            break;
    }

    ctx.fbc[ctx.cur_y * ctx.ch_w + ctx.cur_x] = (fbc_entry){
        c,
        ctx.cur_cols.fg,
        ctx.cur_cols.bg,
    };

    /* Draw the pixels on the screen */
    fbc_refresh_entry(ctx.cur_y, ctx.cur_x);

    /* If we reach the end of the line, reset x and increase y */
    if (++(ctx.cur_x) >= ctx.ch_w) {
        ctx.cur_x = 0;

        /* See comment on newline conditional */
        if (ctx.cur_y + 1 < ctx.ch_h)
            ctx.cur_y++;
        else
            should_shift = true;
    }
}

/* fbc_refresh: updates each pixel of the framebuffer with the real one in
 * ctx.fbc. Calling this function everytime we update ctx.fbc would be slow.
 * Instead call this function on specific situations and we refresh the entries
 * we need when updating ctx.fbc (e.g. when calling fbc_putchar) */
void fbc_refresh(void) {
    /* First iterate each char of the framebuffer console */
    for (uint32_t cy = 0; cy < ctx.ch_h; cy++)
        for (uint32_t cx = 0; cx < ctx.ch_w; cx++)
            fbc_refresh_entry(cy, cx);
}

/* fbc_shift_rows: scrolls the framebuffer terminal "n" rows (fbc_entry's) */
/* TODO: Still not very fast. Optimize */
void fbc_shift_rows(uint8_t n) {
    /* Used to count the position of the last valid char in the line */
    uint32_t char_count = 0;

    /* Shift n rows. We go to the newline instead of always ctx.ch_w because
     * that will be the last valid char we care about. We can fill the rest
     * faster with fb_drawrect_fast */
    for (uint32_t y = 0; y < ctx.ch_h - n; y++) {
        /* Update valid entries until we encounter a null byte. '\0' denotes the
         * end of the valid line. */
        for (uint32_t x = 0; x < ctx.ch_w; x++) {
            ctx.fbc[y * ctx.ch_w + x] = ctx.fbc[(y + n) * ctx.ch_w + x];

            /* We need to check after the assignment and not in the for because
             * we want to also copy the null byte to keep where the line ends */
            if (ctx.fbc[(y + n) * ctx.ch_w + x].c == '\0')
                break;

            fbc_refresh_entry(y, x);

            char_count++;
        }

        /* Fill from last valid to the end of the line */
        const uint32_t fill_y = CHAR_Y_TO_PX(y);
        const uint32_t fill_x = CHAR_X_TO_PX(char_count);
        const uint32_t fill_h = ctx.font->h;
        const uint32_t fill_w = ctx.w + ctx.x - fill_x;
        fb_drawrect_fast(fill_y, fill_x, fill_h, fill_w, DEFAULT_BG);

        char_count = 0;
    }

    /* Clear last n rows with clean entries. Only change the ones that were
     * full */
    for (uint32_t y = ctx.ch_h - n; y < ctx.ch_h; y++) {
        /* First entry is newline, rest spaces. We dont need to call
         * fbc_refresh_entry because we know the whole line is empty */
        ctx.fbc[y * ctx.ch_w + 0] = (fbc_entry){
            '\n',
            DEFAULT_FG,
            DEFAULT_BG,
        };

        for (uint32_t x = 1; x < ctx.ch_w; x++) {
            ctx.fbc[y * ctx.ch_w + x] = (fbc_entry){
                '\0',
                DEFAULT_FG,
                DEFAULT_BG,
            };
        }
    }

    /* Fill last empty lines with background color */
    const uint32_t fill_y = CHAR_Y_TO_PX(ctx.ch_h - n);
    const uint32_t fill_x = CHAR_X_TO_PX(0);
    const uint32_t fill_h = n * ctx.font->h;
    const uint32_t fill_w = ctx.w;
    fb_drawrect_fast(fill_y, fill_x, fill_h, fill_w, DEFAULT_BG);
}

/* -------------------------------------------------------------------------- */

/* fbc_getcols: writes the current colors of the terminal to "fg" and "bg" */
void fbc_getcols(uint32_t* fg, uint32_t* bg) {
    *fg = ctx.cur_cols.fg;
    *bg = ctx.cur_cols.bg;
}

/* fbc_setcol: sets the current foreground and background colors */
void fbc_setcol(uint32_t fg, uint32_t bg) {
    ctx.cur_cols.fg = fg;
    ctx.cur_cols.bg = bg;
}

/* fbc_setfore: change the current foreground color */
void fbc_setfore(uint32_t fg) {
    ctx.cur_cols.fg = fg;
}

/* fbc_setback: change the current background color */
void fbc_setback(uint32_t bg) {
    ctx.cur_cols.bg = bg;
}

/* fbc_setcol_rgb: sets the current foreground and background colors in rgb
 * format */
void fbc_setcol_rgb(uint8_t fore_r, uint8_t fore_g, uint8_t fore_b,
                    uint8_t back_r, uint8_t back_g, uint8_t back_b) {
    ctx.cur_cols.fg = rgb2col(fore_r, fore_g, fore_b);
    ctx.cur_cols.bg = rgb2col(back_r, back_g, back_b);
}

