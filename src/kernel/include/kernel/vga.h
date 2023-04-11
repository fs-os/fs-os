
#ifndef _KERNEL_VGA_H
#define _KERNEL_VGA_H

#include <stddef.h> /* size_t */
#include <stdint.h> /* uintX_t */

/**
 * @enum vga_color
 * @brief Colors for the vga console entries.
 */
enum vga_color {
    VGA_COLOR_BLACK         = 0,  /**< Black          */
    VGA_COLOR_BLUE          = 1,  /**< Blue           */
    VGA_COLOR_GREEN         = 2,  /**< Green          */
    VGA_COLOR_CYAN          = 3,  /**< Cyan           */
    VGA_COLOR_RED           = 4,  /**< Red            */
    VGA_COLOR_MAGENTA       = 5,  /**< Magenta        */
    VGA_COLOR_BROWN         = 6,  /**< Brown          */
    VGA_COLOR_LIGHT_GREY    = 7,  /**< Light grey     */
    VGA_COLOR_DARK_GREY     = 8,  /**< Dark grey      */
    VGA_COLOR_LIGHT_BLUE    = 9,  /**< Light blue     */
    VGA_COLOR_LIGHT_GREEN   = 10, /**< Light green    */
    VGA_COLOR_LIGHT_CYAN    = 11, /**< Light cyan     */
    VGA_COLOR_LIGHT_RED     = 12, /**< Light red      */
    VGA_COLOR_LIGHT_MAGENTA = 13, /**< Light magenta  */
    VGA_COLOR_LIGHT_BROWN   = 14, /**< Light brown    */
    VGA_COLOR_WHITE         = 15, /**< White          */
};

/**
 * @def VGA_CONSOLE_ADDR
 * @brief Address of the VGA console.
 */
#define VGA_CONSOLE_ADDR 0xB8000

/**
 * @name Width and height of the VGA console.
 * @brief Defined in vga.c
 * @{ */
extern const size_t VGA_WIDTH;  /**< Description */
extern const size_t VGA_HEIGHT; /**< Description */
/** @} */

/**
 * @brief Get vga color pair from foreground and background colors.
 * @details Foreground will be bits 0..3 and background bits 4..7
 * @param[in] fg Foreground color from the vga_color enum.
 * @param[in] bg Background color from the vga_color enum.
 * @return 8 bit VGA color pair.
 */
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

/**
 * @brief Get vga entry from char and color pair
 * @param[in] uc Char for the VGA entry.
 * @param[in] color 8 bit VGA color returned from vga_entry_color()
 * @return VGA entry with the char and color.
 */
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

/**
 * @brief Inits the terminal x, y, color and sets the term buffer to grey spaces
 */
void vga_init(void);

/**
 * @brief changes the current color for the terminal from a vga entry
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
 * location in the vga terminal.
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
 * @brief Prints `len` bytes of `s` to the vga terminal
 * @param[in] s Char array to be writen.
 * @param[in] len The number of bytes to print.
 */
void vga_write(const char* s, size_t len);

/**
 * @brief Prints a zero-terminated string to the vga terminal using vga_write()
 * @details If we used the VGA terminal, we would need to improve this function.
 * @param[in] s Zero-terminated string to print.
 */
void vga_sprint(const char* s);

#endif /* _KERNEL_VGA_H */
