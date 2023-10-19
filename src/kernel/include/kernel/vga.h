
#ifndef KERNEL_VGA_H_
#define KERNEL_VGA_H_ 1

#include <stddef.h>
#include <stdint.h>

/**
 * @enum vga_color
 * @brief Colors for the VGA console entries.
 */
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

/**
 * @def VGA_CONSOLE_ADDR
 * @brief Address of the VGA console.
 */
#define VGA_CONSOLE_ADDR 0xB8000

/**
 * @name Width and height of the VGA console.
 * @brief Defined in vga.c
 */
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

/**
 * @brief Get VGA color pair from foreground and background colors.
 * @details Foreground will be bits 0..3 and background bits 4..7
 * @param[in] fg Foreground color from the vga_color enum.
 * @param[in] bg Background color from the vga_color enum.
 * @return 8 bit VGA color pair.
 */
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | (bg << 4);
}

/**
 * @brief Get VGA entry from char and color pair
 * @param[in] uc Char for the VGA entry.
 * @param[in] color 8 bit VGA color returned from vga_entry_color()
 * @return VGA entry with the char and color.
 */
static inline uint16_t vga_entry(uint8_t uc, uint8_t color) {
    return (uint16_t)uc | ((uint16_t)color << 8);
}

/**
 * @brief Initializes the cursor position in the terminal, colors and clears the
 * terminal buffer to grey spaces.
 */
void vga_init(void);

/**
 * @brief changes the current color for the terminal from a VGA entry
 * @param[in] col VGA color pair returned from vga_entry_color()
 */
void vga_setcol_entry(uint8_t col);

/**
 * @brief Changes the current color for the terminal from 2 color codes
 * @param[in] fg Foreground color from the vga_color enum.
 * @param[in] bg Background color from the vga_color enum.
 */
void vga_setcol(enum vga_color fg, enum vga_color bg);

/**
 * @brief Writes the specified char with the specified color at the specified
 * location in the VGA terminal.
 * @param[in] y, x Position for the new char.
 * @param[in] col 8 bit VGA color pair for the new char.
 * @param[in] c Char to put in that location.
 */
void vga_put_at(size_t y, size_t x, uint8_t col, char c);

/**
 * @brief Scrolls the terminal n rows.
 * @param[in] n The number of rows to shift.
 */
void vga_shift_rows(int n);

/**
 * @brief Prints the specified char to the VGA terminal.
 * @param[in] c Char to be printed.
 */
void vga_putchar(char c);

/**
 * @brief Prints a zero-terminated string to the VGA terminal using vga_write()
 * @details If we used the VGA terminal, we would need to improve this function.
 * @param[in] s Zero-terminated string to print.
 */
void vga_print(const char* s);

#endif /* KERNEL_VGA_H_ */
