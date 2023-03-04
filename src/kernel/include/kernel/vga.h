
#ifndef _KERNEL_VGA_H
#define _KERNEL_VGA_H

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

#define VGA_CONSOLE_ADDR 0xB8000

extern const size_t VGA_WIDTH;
extern const size_t VGA_HEIGHT;

/* vga_entry_color: get vga color pair from foreground and background colors */
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    /*
     * Shifts the background color 4 bytes and ORs it with the foreground color.
     *
     * fg  = b00001010
     * bg  = b00001100
     *       ---------
     * ret = b11001010
     */
    return fg | bg << 4;
}

/* vga_entry: get vga entry from char and color pair (from vga_entry_color) */
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    /*
     * Shifts the color from vga_entry_color 8 bytes and ORs it with uc.
     *
     * uc  = b0000000000001010 (10)
     * col = b0000000011001010
     *       -----------------
     * ret = b1100101000001010
     */
    return (uint16_t)uc | (uint16_t)color << 8;
}

/* Inits the terminal x, y, color and sets the term buffer to grey spaces */
void vga_init(void);

/* vga_setcol_entry: changes the current color for the terminal from a vga
 * entry */
void vga_setcol_entry(uint8_t col);

/* vga_setcol: changes the current color for the terminal from 2 color codes */
void vga_setcol(enum vga_color fg, enum vga_color bg);

/* vga_put_at: writes "c" with the color "col" at "x" and "y" in the vga
 * terminal */
void vga_put_at(size_t y, size_t x, uint8_t col, char c);

/* vga_shift_rows: scrolls the terminal n rows */
void vga_shift_rows(int n);

/* vga_putchar: prints 'c' to the vga terminal */
void vga_putchar(char c);

/* vga_write: prints 'len' bytes of 's' to the vga terminal */
void vga_write(const char* s, size_t len);

/* vga_sprint: prints zero-terminated string to the vga terminal using
 * vga_write */
void vga_sprint(const char* s);

#endif /* _KERNEL_VGA_H */

