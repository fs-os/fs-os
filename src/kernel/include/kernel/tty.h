
#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h> /* size_t */
#include <stdint.h> /* uintX_t */

enum vga_color {
    VGA_COLOR_BLACK         = 0,
    VGA_COLOR_BLUE          = 1,
    VGA_COLOR_GREEN         = 2,
    VGA_COLOR_CYAN          = 3,
    VGA_COLOR_RED           = 4,
    VGA_COLOR_MAGENTA       = 5,
    VGA_COLOR_BROWN         = 6,
    VGA_COLOR_LIGHT_GREY    = 7,
    VGA_COLOR_DARK_GREY     = 8,
    VGA_COLOR_LIGHT_BLUE    = 9,
    VGA_COLOR_LIGHT_GREEN   = 10,
    VGA_COLOR_LIGHT_CYAN    = 11,
    VGA_COLOR_LIGHT_RED     = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN   = 14,
    VGA_COLOR_WHITE         = 15,
};

extern const size_t VGA_WIDTH;
extern const size_t VGA_HEIGHT;

extern size_t term_y;
extern size_t term_x;
extern uint8_t term_col;   /* Color */
extern uint16_t* term_buf; /* Buffer of vga_entries */

/* Inits the terminal x, y, color and sets the term buffer to grey spaces */
void term_init();

/* term_setcol_entry: changes the current color for the terminal from a vga entry */
void term_setcol_entry(uint8_t col);

/* term_setcol: changes the current color for the terminal from 2 color codes */
void term_setcol(enum vga_color fg, enum vga_color bg);

/* term_put_at: writes "c" with the color "col" at "x" and "y" in the vga terminal */
void term_put_at(size_t y, size_t x, uint8_t col, char c);

/* shift_rows: scrolls the terminal n rows */
void shift_rows(int n);

/* term_putchar: prints 'c' to the vga terminal */
void term_putchar(char c);

/* term_write: prints 'len' bytes of 's' to the vga terminal */
void term_write(const char* s, size_t len);

/* term_sprint: prints zero-terminated string to the vga terminal using term_write */
void term_sprint(const char* s);

#endif
