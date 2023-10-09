
#ifndef KERNEL_FRAMEBUFFER_CONSOLE_H_
#define KERNEL_FRAMEBUFFER_CONSOLE_H_ 1

#include <stdint.h>
#include <stddef.h>
#include <kernel/font.h>
#include <kernel/color.h> /* color_pair */

/**
 * @brief Number of spaces per tab to be displayed
 */
#define FBC_TABSIZE 4

/**
 * @brief Framebuffer console entry.
 */
typedef struct {
    uint8_t c;   /**< @brief Char of the entry */
    uint32_t fg; /**< @brief 32 bit foreground color */
    uint32_t bg; /**< @brief 32 bit background color */
} fbc_entry;

/**
 * @brief Framebuffer console context.
 * @details Used for example by ncurses.
 */
typedef struct {
    /** @brief Global framebuffer console. Main fbc_entry array */
    fbc_entry* fbc;

    /** @name Global size in px of the framebuffer console
     * @{ */
    uint32_t y, x, h, w;
    /** @} */

    /** @name Global number of entry rows and cols that form the console (chars)
     * @{ */
    uint32_t ch_h, ch_w;
    /** @} */

    /** @brief Global pointer to the font the framebufer console is using */
    Font* font;

    /** @brief Current color_pair we are using when printing. Foreground and
     * background */
    color_pair cur_cols;

    /** @name Current character position on the console
     * @{ */
    uint32_t cur_y, cur_x;
    /** @} */

    /** @brief Will be set to true if we know we are going to shift the
     * console of this context. See framebuffer console wiki page. */
    bool should_shift;
} fbc_ctx;

/**
 * @brief Initialize the framebuffer console
 * @param y, x Position of the new console in px
 * @param h, w Size of the new console in px
 * @param[in] font Pointer to the font
 */
void fbc_init(uint32_t y, uint32_t x, uint32_t h, uint32_t w, Font* font);

/**
 * @brief Switches to the specified framebuffer console context
 * @param[in] new_ctx New framebuffer console context
 */
void fbc_change_ctx(fbc_ctx* new_ctx);

/**
 * @brief Get the current framebuffer console context
 * @return Current framebuffer console context
 */
fbc_ctx* fbc_get_ctx(void);

/**
 * @brief Clears the framebuffer console and moves cursor to the first char
 */
void fbc_clear(void);

/**
 * @brief Clear to end of line
 * @details Does not change cursor position
 */
void fbc_clrtoeol(void);

/**
 * @brief prints zero-terminated string to the framebuffer console using
 * fbc_putchar()
 * @param s Zero-terminated string to print
 */
void fbc_sprint(const char* s);

/**
 * @brief Prints \p c to the framebuffer console
 * @param c Char to print
 */
void fbc_putchar(char c);

/**
 * @brief Updates each pixel of the framebuffer with the real one in ctx->fbc.
 * @details Calling this function everytime we update ctx.fbc would be slow.
 * Instead we call this function on specific situations and we refresh the
 * entries we need when updating ctx->fbc (e.g. when calling fbc_putchar())
 *
 * The raw version is usually meant for refreshing everything when we control
 * all the contents (e.g. after clearing the fbc)
 */
void fbc_refresh_raw(void);

/**
 * @brief Same as fbc_refresh_raw(), but stop at end of line.
 * @defails End of line meaning `'\0'`. Usually meant for refreshing when we are
 * not sure of the fbc contents beforehand.
 */
void fbc_refresh(void);

/**
 * @brief Scrolls the framebuffer terminal \p n rows.
 * @param n Number of rows to shift
 */
void fbc_shift_rows(uint8_t n);

/* fbc_getcols:  */
/**
 * @brief Writes the current colors of the terminal to \p fg and \p bg.
 * @param[out] fg Foreground color dst pointer
 * @param[out] bg Background color dst pointer
 */
void fbc_getcols(uint32_t* fg, uint32_t* bg);

/**
 * @brief Sets the specified colors for the terminal.
 * @param[in] fg Foreground color
 * @param[in] bg Background color
 */
void fbc_setcol(uint32_t fg, uint32_t bg);

/**
 * @brief Sets the specified foreground color for the terminal.
 * @param[in] fg Foreground color
 */
void fbc_setfore(uint32_t fg);

/**
 * @brief Sets the specified background color for the terminal.
 * @param[in] bg Background color
 */
void fbc_setback(uint32_t bg);

/**
 * @brief Sets the current foreground and background colors in rgb format
 * @param fore_r, fore_g, fore_b Foreground color in rgb format
 * @param back_r, back_g, back_b Background color in rgb format
 */
void fbc_setcol_rgb(uint8_t fore_r, uint8_t fore_g, uint8_t fore_b,
                    uint8_t back_r, uint8_t back_g, uint8_t back_b);

#endif /* KERNEL_FRAMEBUFFER_CONSOLE_H_ */
