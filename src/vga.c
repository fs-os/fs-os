
#include "vga.h"

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

static const size_t VGA_WIDTH  = 80;
static const size_t VGA_HEIGHT = 25;

size_t term_y;
size_t term_x;
uint8_t term_col;   /* Color */
uint16_t* term_buf; /* Buffer of vga_entries */

/* Inits the terminal x, y, color and sets the term buffer to grey spaces */
void term_init() {
    term_y   = 0;
    term_x   = 0;
    term_col = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    /* 0xB8000 is the location of the VGA text mode buffer */
    term_buf = (uint16_t*)0xB8000;

    /* Clear terminal buffer */
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            /* 2D coordinates to 1D array idx */
            term_buf[y * VGA_WIDTH + x] = vga_entry(' ', term_col);
        }
    }
}

/* term_setcol: changes the current color for the terminal */
void term_setcol(uint8_t col) {
    term_col = col;
}

/* term_put_at: writes "c" with the color "col" at "x" and "y" in the vga terminal */
void term_put_at(size_t y, size_t x, uint8_t col, char c) {
    term_buf[y * VGA_WIDTH + x] = vga_entry(c, col);
}

/* term_putchar: prints 'c' to the vga terminal */
void term_putchar(char c) {
    /* If we try to put a newline, go to the next line without putting any char */
    if (c == '\n') {
        term_y++;
        term_x = 0;
        return;
    }

    term_put_at(term_y, term_x, term_col, c);

    /* If we reach the end of the line, reset x and increase y */
    if (++term_x == VGA_WIDTH) {
        term_x = 0;

        /* TODO: If we write more than the screen height, shift screen rows */
        if (++term_y == VGA_HEIGHT)
            term_y = 0;
    }
}

/* term_write: prints 'len' bytes of 's' to the vga terminal */
void term_write(const char* s, size_t len) {
    /* No need to create a variable for iteration when we can increment the ptr */
    while (len-- > 0)
        term_putchar(*s++);
}

/* term_sprint: prints zero-terminated string to the vga terminal using term_write */
void term_sprint(const char* s) {
    term_write(s, strlen(s));
}
