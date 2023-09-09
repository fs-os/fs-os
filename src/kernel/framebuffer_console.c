
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <kernel/color.h>
#include <kernel/vga.h> /* VGA_CONSOLE_ADDR */
#include <kernel/framebuffer.h>
#include <kernel/framebuffer_console.h>

/**
 * @brief Converts a char Y position in the fbc to a pixel position
 * @param y Character position in the framebuffer console
 * @return Pixel position in the screen of the char
 */
#define CHAR_Y_TO_PX(fy) (ctx->y + ((fy)*ctx->font->h))

/**
 * @brief Converts a char X position in the fbc to a pixel position
 * @param x Character position in the framebuffer console
 * @return Pixel position in the screen of the char
 */
#define CHAR_X_TO_PX(fx) (ctx->x + ((fx)*ctx->font->w))

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
static fbc_ctx _first_ctx;
static fbc_ctx* ctx = &_first_ctx;

/* -------------------------------------------------------------------------- */

/**
 * @brief Refreshes the pixels on the screen corresponding to the specified fbc
 * character.
 * @param cy, cx Position of the character we want to refresh in the fbc array
 */
static inline void fbc_refresh_entry(uint32_t cy, uint32_t cx) {
    /* Get the current fbc_entry */
    const fbc_entry cur_entry = ctx->fbc[cy * ctx->ch_w + cx];
    volatile uint32_t* fb_ptr = fb_get_ptr();
    const uint32_t fb_w       = fb_get_width();

    /* Then iterate each pixel that forms the font char */
    for (uint8_t fy = 0; fy < ctx->font->h; fy++) {
        for (uint8_t fx = 0; fx < ctx->font->w; fx++) {
            /* Get real screen position from the char offset on the screen and
             * the pixel font offset on the char */
            const uint32_t final_y = CHAR_Y_TO_PX(cy) + fy;
            const uint32_t final_x = CHAR_X_TO_PX(cx) + fx;

            /* We check if the current bit of the current column is 1.
             * Depending on that, set it to foreground or background. For
             * more information see: src/kernel/include/kernel/font.h */
            fb_ptr[final_y * fb_w + final_x] =
              (get_font_bit(ctx->font, cur_entry.c, fy, fx)) ? cur_entry.fg
                                                             : cur_entry.bg;
        }
    }
}

/* -------------------------------------------------------------------------- */

void fbc_init(uint32_t y, uint32_t x, uint32_t h, uint32_t w, Font* font) {
    ctx->y = y;
    ctx->x = x;
    ctx->h = h;
    ctx->w = w;

    /* We get the font size but we save the console char dimensions */
    ctx->ch_h = h / font->h;
    ctx->ch_w = w / font->w;
    ctx->font = font;

    ctx->cur_cols.fg = DEFAULT_FG;
    ctx->cur_cols.bg = DEFAULT_BG;

    ctx->cur_y = 0;
    ctx->cur_x = 0;

    ctx->should_shift = false;

    /* Allocate the number of fbc_entry's. Rows and cols of the console */
    ctx->fbc = malloc(ctx->ch_h * ctx->ch_w * sizeof(fbc_entry));

    fbc_clear();
    fbc_refresh_raw();
}

void fbc_change_ctx(fbc_ctx* new_ctx) {
    ctx = new_ctx;
}

fbc_ctx* fbc_get_ctx(void) {
    return ctx;
}

void fbc_clear(void) {
    ctx->cur_x = 0;
    ctx->cur_y = 0;

    for (uint32_t cy = 0; cy < ctx->ch_h; cy++) {
        /* First entry is newline, rest spaces. We dont need to call
         * fbc_refresh_entry because we know the whole line is empty */
        ctx->fbc[cy * ctx->ch_w + 0] = (fbc_entry){
            .c  = '\n',
            .fg = DEFAULT_FG,
            .bg = DEFAULT_BG,
        };

        for (uint32_t cx = 1; cx < ctx->ch_w; cx++) {
            ctx->fbc[cy * ctx->ch_w + cx] = (fbc_entry){
                .c  = '\0',
                .fg = DEFAULT_FG,
                .bg = DEFAULT_BG,
            };
        }
    }
}

void fbc_clrtoeol(void) {
    /* Current position will be the end of the current line */
    ctx->fbc[ctx->cur_y * ctx->ch_w + ctx->cur_x] = (fbc_entry){
        .c  = '\n',
        .fg = DEFAULT_FG,
        .bg = DEFAULT_BG,
    };
    fbc_refresh_entry(ctx->cur_y, ctx->cur_x);

    /* Rest of them as '\0' */
    for (uint32_t cx = ctx->cur_x + 1; cx < ctx->ch_w; cx++) {
        ctx->fbc[ctx->cur_y * ctx->ch_w + cx] = (fbc_entry){
            .c  = '\0',
            .fg = DEFAULT_FG,
            .bg = DEFAULT_BG,
        };
        fbc_refresh_entry(ctx->cur_y, cx);
    }
}

void fbc_sprint(const char* s) {
    while (*s > '\0')
        fbc_putchar(*s++);
}

void fbc_putchar(char c) {
    /* First of all, check if we need to shift the array. We need this kind of
     * "queue" system so the array doesn't immediately shift when a line ends
     * with '\n', for example */
    if (ctx->should_shift) {
        fbc_shift_rows(1);
        ctx->should_shift = false;
    }

    /* Check for special chars like newlines, tabs, backspaces, etc. */
    switch (c) {
        case '\n':
            /* Save newline char (don't display anything) */
            ctx->fbc[ctx->cur_y * ctx->ch_w + ctx->cur_x] = (fbc_entry){
                .c  = '\n',
                .fg = DEFAULT_FG,
                .bg = DEFAULT_BG,
            };

            /* If we have rows left on the terminal, go down, if we are on the
             * last one, store that we need to shift 1 up, but stay on that last
             * row. See comment on top of this function */
            if (ctx->cur_y + 1 < ctx->ch_h)
                ctx->cur_y++;
            else
                ctx->should_shift = true;

            ctx->cur_x = 0;

            return;
        case '\t':
            /* For having TABSIZE-aligned tabs */
            const int tabs_needed = FBC_TABSIZE - (ctx->cur_x % FBC_TABSIZE);
            for (int i = 0; i < tabs_needed; i++)
                fbc_putchar(' ');

            return;
        case '\b':
            if (ctx->cur_x > 0) {
                ctx->cur_x--;
            } else if (ctx->cur_y > 0) {
                ctx->cur_y--;
                ctx->cur_x = ctx->ch_w - 1;

                while (ctx->fbc[ctx->cur_y * ctx->ch_w + ctx->cur_x].c == '\0')
                    ctx->cur_x--;
            }

            /* Clear the char we just deleted */
            ctx->fbc[ctx->cur_y * ctx->ch_w + ctx->cur_x] = (fbc_entry){
                .c  = ' ',
                .fg = DEFAULT_FG,
                .bg = DEFAULT_BG,
            };

            /* Draw the pixels on the screen */
            fbc_refresh_entry(ctx->cur_y, ctx->cur_x);

            return;
        case '\r':
            /* Fill from start of the line to the cursor pos */
            const uint32_t fill_y = CHAR_Y_TO_PX(ctx->cur_y);
            const uint32_t fill_x = CHAR_X_TO_PX(0);
            const uint32_t fill_h = ctx->font->h;
            const uint32_t fill_w = CHAR_X_TO_PX(ctx->cur_x) - fill_x;
            fb_drawrect_fast(fill_y, fill_x, fill_h, fill_w, ctx->cur_cols.bg);

            for (uint32_t tmp_x = 0; tmp_x < ctx->cur_x; tmp_x++) {
                ctx->fbc[ctx->cur_y * ctx->ch_w + tmp_x] = (fbc_entry){
                    .c  = ' ',
                    .fg = ctx->cur_cols.fg,
                    .bg = ctx->cur_cols.bg,
                };
            }

            ctx->cur_x = 0;
            return;
        default:
            break;
    }

    ctx->fbc[ctx->cur_y * ctx->ch_w + ctx->cur_x] = (fbc_entry){
        .c  = c,
        .fg = ctx->cur_cols.fg,
        .bg = ctx->cur_cols.bg,
    };

    /* Draw the pixels on the screen */
    fbc_refresh_entry(ctx->cur_y, ctx->cur_x);

    /* If we reach the end of the line, reset x and increase y */
    if (++(ctx->cur_x) >= ctx->ch_w) {
        ctx->cur_x = 0;

        /* See comment on newline conditional */
        if (ctx->cur_y + 1 < ctx->ch_h)
            ctx->cur_y++;
        else
            ctx->should_shift = true;
    }
}

void fbc_refresh_raw(void) {
    /* Iterate each char of the framebuffer console */
    for (uint32_t cy = 0; cy < ctx->ch_h; cy++)
        for (uint32_t cx = 0; cx < ctx->ch_w; cx++)
            fbc_refresh_entry(cy, cx);
}

void fbc_refresh(void) {
    /* Iterate each char of the framebuffer console */
    for (uint32_t cy = 0; cy < ctx->ch_h; cy++) {
        for (uint32_t cx = 0; cx < ctx->ch_w; cx++) {
            fbc_refresh_entry(cy, cx);

            /* If NULL, we are done with the line: Fill from current pos to end
             * of line and go to the next one */
            if (ctx->fbc[cy * ctx->ch_w + cx].c == '\0') {
                const uint32_t fill_y = CHAR_Y_TO_PX(cy);
                const uint32_t fill_x = CHAR_X_TO_PX(cx);
                const uint32_t fill_h = ctx->font->h;
                const uint32_t fill_w = CHAR_X_TO_PX(ctx->ch_w) - fill_x;
                fb_drawrect_fast(fill_y, fill_x, fill_h, fill_w,
                                 ctx->cur_cols.bg);
                break;
            }
        }
    }
}

void fbc_shift_rows(uint8_t n) {
    /* Get once for performance. The last_row variable is the last row that we
     * want to replace after shifting. */
    const uint32_t last_row = ctx->ch_h - n - 1;

    /* Shift N rows. We go to the newline instead of always ctx->ch_w because
     * that will be the last valid char we care about. We can fill the rest
     * faster with fb_drawrect_fast */
    for (uint32_t y = 0; y <= last_row; y++) {
        /* Get once for performance. Used for ctx->fbc[] indexes */
        const uint32_t raw_y        = y * ctx->ch_w;
        const uint32_t raw_y_plus_n = (y + n) * ctx->ch_w;

        /* Used to count the position of the last valid char in the line */
        uint32_t char_count = 0;

        /* Update valid entries until we encounter a null byte. '\0' denotes the
         * end of the valid line. */
        for (uint32_t x = 0; x < ctx->ch_w; x++) {
            ctx->fbc[raw_y + x] = ctx->fbc[raw_y_plus_n + x];

            /* We need to check after the assignment and not in the for because
             * we want to also copy the null byte to keep where the line ends */
            if (ctx->fbc[raw_y_plus_n + x].c == '\0')
                break;

            fbc_refresh_entry(y, x);

            char_count++;
        }

        /* Fill from last valid to the end of the line */
        const uint32_t fill_y = CHAR_Y_TO_PX(y);
        const uint32_t fill_x = CHAR_X_TO_PX(char_count);
        const uint32_t fill_w = ctx->w + ctx->x - fill_x;
        fb_drawrect_fast(fill_y, fill_x, ctx->font->h, fill_w, DEFAULT_BG);
    }

    /* Clear last n rows with clean entries. Only change the ones that were
     * full. The y variable starts as the first empty line after shifting */
    for (uint32_t y = last_row + 1; y < ctx->ch_h; y++) {
        /* Get once for performance. Used for ctx->fbc[] indexes */
        const uint32_t raw_y = y * ctx->ch_w;

        /* First entry is newline, rest spaces. We dont need to call
         * fbc_refresh_entry because we know the whole line is empty */
        ctx->fbc[raw_y + 0] = (fbc_entry){
            .c  = '\n',
            .fg = DEFAULT_FG,
            .bg = DEFAULT_BG,
        };

        for (uint32_t x = 1; x < ctx->ch_w; x++) {
            ctx->fbc[raw_y + x] = (fbc_entry){
                .c  = '\0',
                .fg = DEFAULT_FG,
                .bg = DEFAULT_BG,
            };
        }
    }

    /* Fill last empty lines with background color */
    const uint32_t fill_y = CHAR_Y_TO_PX(ctx->ch_h - n);
    const uint32_t fill_x = CHAR_X_TO_PX(0);
    const uint32_t fill_h = n * ctx->font->h;
    fb_drawrect_fast(fill_y, fill_x, fill_h, ctx->w, DEFAULT_BG);
}

/* -------------------------------------------------------------------------- */

void fbc_getcols(uint32_t* fg, uint32_t* bg) {
    *fg = ctx->cur_cols.fg;
    *bg = ctx->cur_cols.bg;
}

void fbc_setcol(uint32_t fg, uint32_t bg) {
    ctx->cur_cols.fg = fg;
    ctx->cur_cols.bg = bg;
}

void fbc_setfore(uint32_t fg) {
    ctx->cur_cols.fg = fg;
}

void fbc_setback(uint32_t bg) {
    ctx->cur_cols.bg = bg;
}

void fbc_setcol_rgb(uint8_t fore_r, uint8_t fore_g, uint8_t fore_b,
                    uint8_t back_r, uint8_t back_g, uint8_t back_b) {
    ctx->cur_cols.fg = rgb2col(fore_r, fore_g, fore_b);
    ctx->cur_cols.bg = rgb2col(back_r, back_g, back_b);
}
