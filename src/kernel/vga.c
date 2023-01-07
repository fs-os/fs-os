
#include <string.h>
#include <kernel/vga.h>

const size_t VGA_WIDTH  = 80;
const size_t VGA_HEIGHT = 25;

size_t term_y;
size_t term_x;
uint8_t term_col;   /* Color */
uint16_t* term_buf; /* Buffer of vga_entries */

/* Inits the terminal x, y, color and sets the term buffer to grey spaces */
void term_init(void) {
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

/* term_setcol_entry: changes the current color for the terminal from a vga entry */
void term_setcol_entry(uint8_t col) {
    term_col = col;
}

/* term_setcol: changes the current color for the terminal from 2 color codes */
void term_setcol(enum vga_color fg, enum vga_color bg) {
    term_col = vga_entry_color(fg, bg);
}

/* term_put_at: writes "c" with the color "col" at "x" and "y" in the vga terminal */
void term_put_at(size_t y, size_t x, uint8_t col, char c) {
    term_buf[y * VGA_WIDTH + x] = vga_entry(c, col);
}

/* term_shift_rows: scrolls the terminal n rows */
void term_shift_rows(int n) {
    /* Shift n rows */
    for (size_t y = 0; y < VGA_HEIGHT - n; y++)
        for (size_t x = 0; x < VGA_WIDTH; x++)
            term_buf[y * VGA_WIDTH + x] = term_buf[(y + n) * VGA_WIDTH + x];

    /* Clear last n rows with the current color (only the background matters) */
    for (size_t y = VGA_HEIGHT - n; y < VGA_HEIGHT; y++)
        for (size_t x = 0; x < VGA_WIDTH; x++)
            term_buf[y * VGA_WIDTH + x] = vga_entry(' ', term_col);
}

/* term_putchar: prints 'c' to the vga terminal */
void term_putchar(char c) {
    /* If we try to put a newline, go to the next line without putting any char */
    if (c == '\n') {
        /* If we have rows left on the terminal, go down, if we are on the last one,
         * shift 1 up, but stay on that last row */
        if (term_y + 1 < VGA_HEIGHT)
            term_y++;
        else
            term_shift_rows(1);

        term_x = 0;

        return;
    }

    term_put_at(term_y, term_x, term_col, c);

    /* If we reach the end of the line, reset x and increase y */
    if (++term_x >= VGA_WIDTH) {
        term_x = 0;

        /* See comment on newline conditional */
        if (term_y + 1 < VGA_HEIGHT)
            term_y++;
        else
            term_shift_rows(1);
    }
}

/* term_write: prints 'len' bytes of 's' to the vga terminal */
void term_write(const char* s, size_t len) {
    /* No need to create a variable for iteration when we can increment the ptr */
    while (len-- > 0)
        term_putchar(*s++);
}

/* term_sprint: prints zero-terminated string to the vga terminal using term_write */
/* TODO: If we actually used vga, it would be better to write until '\0' instead of
 * calling strlen */
void term_sprint(const char* s) {
    term_write(s, strlen(s));
}

