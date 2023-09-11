
#include <string.h>
#include <kernel/vga.h>

const size_t VGA_WIDTH  = 80;
const size_t VGA_HEIGHT = 25;

static size_t term_y;      /**< @brief Current Y position in the VGA console */
static size_t term_x;      /**< @brief Current X position in the VGA console */
static uint8_t term_col;   /**< @brief Color */
static uint16_t* term_buf; /**< @brief Buffer of vga_entries */

void vga_init(void) {
    term_y   = 0;
    term_x   = 0;
    term_col = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    /* 0xB8000 is the location of the VGA text mode buffer */
    term_buf = (uint16_t*)VGA_CONSOLE_ADDR;

    /* Clear terminal buffer */
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            /* 2D coordinates to 1D array idx */
            term_buf[y * VGA_WIDTH + x] = vga_entry(' ', term_col);
        }
    }
}

void vga_setcol_entry(uint8_t col) {
    term_col = col;
}

void vga_setcol(enum vga_color fg, enum vga_color bg) {
    term_col = vga_entry_color(fg, bg);
}

void vga_put_at(size_t y, size_t x, uint8_t col, char c) {
    term_buf[y * VGA_WIDTH + x] = vga_entry(c, col);
}

void vga_shift_rows(int n) {
    /* Shift n rows */
    for (size_t y = 0; y < VGA_HEIGHT - n; y++)
        for (size_t x = 0; x < VGA_WIDTH; x++)
            term_buf[y * VGA_WIDTH + x] = term_buf[(y + n) * VGA_WIDTH + x];

    /* Clear last n rows with the current color (only the background matters) */
    for (size_t y = VGA_HEIGHT - n; y < VGA_HEIGHT; y++)
        for (size_t x = 0; x < VGA_WIDTH; x++)
            term_buf[y * VGA_WIDTH + x] = vga_entry(' ', term_col);
}

void vga_putchar(char c) {
    /* If we try to put a newline, go to the next line without putting any
     * char */
    if (c == '\n') {
        /* If we have rows left on the terminal, go down, if we are on the last
         * one, shift 1 up, but stay on that last row */
        if (term_y + 1 < VGA_HEIGHT)
            term_y++;
        else
            vga_shift_rows(1);

        term_x = 0;

        return;
    }

    vga_put_at(term_y, term_x, term_col, c);

    /* If we reach the end of the line, reset x and increase y */
    if (++term_x >= VGA_WIDTH) {
        term_x = 0;

        /* See comment on newline conditional */
        if (term_y + 1 < VGA_HEIGHT)
            term_y++;
        else
            vga_shift_rows(1);
    }
}

void vga_print(const char* s) {
    for (; *s != '\0'; s++)
        vga_putchar(*s);
}
