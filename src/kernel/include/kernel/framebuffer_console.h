
#ifndef _KERNEL_FRAMEBUFFER_CONSOLE_H
#define _KERNEL_FRAMEBUFFER_CONSOLE_H

#include <stdint.h>
#include <stddef.h>
#include <kernel/font.h>
#include <kernel/color.h> /* color_pair */

#define FBC_TABSIZE 4

/* Framebuffer console entry. Char and rgb fg and bg in uint32_t format */
typedef struct {
    uint8_t c;
    uint32_t fg;
    uint32_t bg;
} fbc_entry;

/* Framebuffer console context. Used for example by ncurses. */
typedef struct {
    /* Global framebuffer console. Main fbc_entry array */
    fbc_entry* fbc;
    /* Global size in px of the framebuffer console */
    uint32_t y, x, h, w;
    /* Global number of entry rows and cols that form the console (chars) */
    uint32_t ch_h, ch_w;
    /* Global pointer to the font the framebufer console is using */
    Font* font;
    /* Current color we are using when printing. Foreground and background */
    color_pair cur_cols;
    /* Current position on the console */
    uint32_t cur_y, cur_x;
} fbc_ctx;

/* fbc_init: initialize the framebuffer console. First 4 parameters are position
 * and size in pixels of the console and the last one is the font. The font ptr
 * is stored and used to get the height and width of each char. */
void fbc_init(uint32_t y, uint32_t x, uint32_t h, uint32_t w, Font* font);

/* fbc_clear: clears the framebuffer console and moves cursor to the first
 * char */
void fbc_clear(void);

/* fbc_write: prints "len" of "s" to the framebuffer console using
 * fbc_putchar */
void fbc_write(const char* s, size_t len);

/* fbc_sprint: prints zero-terminated string to the framebuffer console using
 * fbc_putchar */
void fbc_sprint(const char* s);

/* fbc_pace_str: place "str" with current colors at positions (y, x) of the
 * framebuffer console array. Warning: Overwrites! */
void fbc_place_str(uint32_t y, uint32_t x, const char* str);

/* fbc_putchar: prints "c" to the framebuffer console */
void fbc_putchar(char c);

/* fbc_refresh: updates each pixel of the framebuffer with the real one in
 * ctx.fbc. Calling this function everytime we update ctx.fbc would be slow.
 * Instead call this function on specific situations and we refresh the entries
 * we need when updating ctx.fbc (e.g. when calling fbc_putchar) */
void fbc_refresh(void);

/* fbc_shift_rows: scrolls the framebuffer terminal "n" rows (fbc_entry's) */
void fbc_shift_rows(uint8_t n);

/* fbc_getcols: writes the current colors of the terminal to "fg" and "bg" */
void fbc_getcols(uint32_t* fg, uint32_t* bg);

/* fbc_setcol: sets the current foreground and background colors */
void fbc_setcol(uint32_t fg, uint32_t bg);

/* fbc_setfore: change the current foreground color */
void fbc_setfore(uint32_t fg);

/* fbc_setback: change the current background color */
void fbc_setback(uint32_t bg);

/* fbc_setcol_rgb: sets the current foreground and background colors in rgb
 * format */
void fbc_setcol_rgb(uint8_t fore_r, uint8_t fore_g, uint8_t fore_b,
                    uint8_t back_r, uint8_t back_g, uint8_t back_b);

#endif /* _KERNEL_FRAMEBUFFER_CONSOLE_H */

