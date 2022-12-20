
/*
 * For more information see:
 *   https://wiki.osdev.org/Bare_Bones#Implementing_the_Kernel
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
#error "You are not using a cross compiler." \
    "For more information see: https://github.com/fs-os/cross-compiler"
#endif

#if !defined(__i386__)
#error "You are not using a ix86-elf compiler." \
    "For more information see: https://github.com/fs-os/cross-compiler"
#endif

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

/* vga_entry_color: get  */
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

/* TODO: Move to inlcude? */
size_t strlen(const char* str) {
    size_t ret = 0;

    while (str[ret] != '\0')
        ret++;

    return ret;
}

static const size_t VGA_WIDTH  = 80;
static const size_t VGA_HEIGHT = 25;

size_t term_y;
size_t term_x;
uint8_t term_col;   /* Color */
uint16_t* term_buf; /* Buffer of vga_entries */

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

void term_setcol(uint8_t col) {
    term_col = col;
}

void term_put_at(size_t y, size_t x, uint8_t col, char c) {
    term_buf[y * VGA_WIDTH + x] = vga_entry(c, col);
}

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

void term_write(const char* s, size_t len) {
    /* No need to create a variable for iteration when we can increment the ptr */
    while (len-- > 0)
        term_putchar(*s++);
}

void term_sprint(const char* s) {
    term_write(s, strlen(s));
}

/* kernel_main: Called by boot.asm */
void kernel_main() {
    term_init();

    term_sprint("Hello, wellcome to the Free and Simple Operative System!\n"
                "This project is still being developed. For more information, see:\n"
                "https://github.com/fs-os/fs-os");
}

