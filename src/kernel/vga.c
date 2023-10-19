
#include <string.h>
#include <kernel/vga.h>

/** @brief Current position in the VGA console */
static size_t term_y = 0, term_x = 0;

/** @brief Current color from when for drawing in VGA console. See vga_color */
static uint8_t term_col = 0;

/** @brief Pointer to the vga_entry() buffer located at VGA_CONSOLE_ADDR */
static uint16_t* term_buf = NULL;

/*----------------------------------------------------------------------------*/

void vga_init(void) {
    term_y   = 0;
    term_x   = 0;
    term_col = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    /* Defined in vga.h, 0xB8000 is the location of the VGA text mode buffer */
    term_buf = (uint16_t*)VGA_CONSOLE_ADDR;

    /* Clear terminal buffer */
    for (int y = 0; y < VGA_HEIGHT; y++)
        for (int x = 0; x < VGA_WIDTH; x++)
            term_buf[y * VGA_WIDTH + x] = vga_entry(' ', term_col);
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
    for (int y = 0; y < VGA_HEIGHT - n; y++)
        for (int x = 0; x < VGA_WIDTH; x++)
            term_buf[y * VGA_WIDTH + x] = term_buf[(y + n) * VGA_WIDTH + x];

    /* Clear last n rows with the current color (only the background matters) */
    for (int y = VGA_HEIGHT - n; y < VGA_HEIGHT; y++)
        for (int x = 0; x < VGA_WIDTH; x++)
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
