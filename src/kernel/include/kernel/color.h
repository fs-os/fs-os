
#ifndef KERNEL_COLOR_H_
#define KERNEL_COLOR_H_ 1

#include <stdint.h>

/**
 * @def HEADER_PIXEL
 * @brief Extract pixel to byte array from data array exported from GIMP.
 * @details This is a GIMP macro.
 * @param[inout] data Pointer to the C array exported by GIMP. Will increase the
 * pointer by 4 each call
 * @param[out] Array of 3 uint8_t for storing current rgb values
 */
#define GIMP_GET_PIXEL(data, pixel)                                         \
    do {                                                                    \
        pixel[0] = (((data[0] - 33) << 2) | ((data[1] - 33) >> 4));         \
        pixel[1] = ((((data[1] - 33) & 0xF) << 4) | ((data[2] - 33) >> 2)); \
        pixel[2] = ((((data[2] - 33) & 0x3) << 6) | ((data[3] - 33)));      \
        data += 4;                                                          \
    } while (0);

/**
 * @brief Color pair structure with foreground and background
 */
typedef struct {
    uint32_t fg; /**< @brief Foreground */
    uint32_t bg; /**< @brief Background */
} color_pair;

/**
 * @brief Convert rgb values to 32 bit color.
 * @param[in] r Red color. 0-255.
 * @param[in] g Green color. 0-255.
 * @param[in] b Blue color. 0-255.
 * @return 32 bit color (0xRRGGBB)
 */
static inline uint32_t rgb2col(uint8_t r, uint8_t g, uint8_t b) {
    return (r << 16 | g << 8 | b);
}

/**
 * @brief Convert 32 bit color to rgb values.
 * @param[out] r Red color pointer.
 * @param[out] g Green color pointer.
 * @param[out] b Blue color pointer.
 * @param[in] col 32 bit color.
 */
static inline void col2rgb(uint8_t* r, uint8_t* g, uint8_t* b, uint32_t col) {
    *r = (col >> 16) & 0xff;
    *g = (col >> 8) & 0xff;
    *b = col & 0xff;
}

/**
 * @name Color palette
 * @todo Make all hex numbers uppercase in color.h
 * @{ */
#ifndef COLOR_BLACK
#define COLOR_BLACK 0x000000
#endif

#ifndef COLOR_RED
#define COLOR_RED 0xcc2828
#endif

#ifndef COLOR_GREEN
#define COLOR_GREEN 0x44e544
#endif

#ifndef COLOR_YELLOW
#define COLOR_YELLOW 0xdbc51c
#endif

#ifndef COLOR_BLUE
#define COLOR_BLUE 0x274dca
#endif

#ifndef COLOR_MAGENTA
#define COLOR_MAGENTA 0x7547b2
#endif

#ifndef COLOR_PURPLE
#define COLOR_PURPLE COLOR_MAGENTA
#endif

#ifndef COLOR_CYAN
#define COLOR_CYAN 0x56d8d4
#endif

#ifndef COLOR_GRAY
#define COLOR_GRAY 0xa5a5a5
#endif

#ifndef COLOR_WHITE
#define COLOR_WHITE 0xd9d9d9
#endif

/* Bold colors */
#ifndef COLOR_BLACK_B
#define COLOR_BLACK_B 0x333333
#endif

#ifndef COLOR_RED_B
#define COLOR_RED_B 0xff1c23
#endif

#ifndef COLOR_GREEN_B
#define COLOR_GREEN_B 0x23ff1c
#endif

#ifndef COLOR_YELLOW_B
#define COLOR_YELLOW_B 0xfff71c
#endif

#ifndef COLOR_BLUE_B
#define COLOR_BLUE_B 0x1818f9
#endif

#ifndef COLOR_MAGENTA_B
#define COLOR_MAGENTA_B 0xae81ff
#endif

#ifndef COLOR_PURPLE_B
#define COLOR_PURPLE_B COLOR_MAGENTA_B
#endif

#ifndef COLOR_CYAN_B
#define COLOR_CYAN_B 0x1cfff7
#endif

#ifndef COLOR_GRAY_B
#define COLOR_GRAY_B 0x7f7f7f
#endif

#ifndef COLOR_WHITE_B
#define COLOR_WHITE_B 0xffffff
#endif
/**  @} */

/**
 * @name Default foreground and background colors
 *
 * For terminals, curses, etc
 * @{ */
#ifndef DEFAULT_FG
#define DEFAULT_FG COLOR_WHITE
#endif

#ifndef DEFAULT_BG
#define DEFAULT_BG COLOR_BLACK
#endif
/**  @} */

#endif /* KERNEL_COLOR_H_ */
