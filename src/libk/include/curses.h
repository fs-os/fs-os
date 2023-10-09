
/*
 * TODO: Add functions:
 *  - wprintw   (window printf)
 *  - mvwprintw (move & window printf)
 *  - waddch    (window putchar)
 *  - mvwaddch  (move & window putchar)
 *
 * TODO (Long term):
 *  - keypad    (arrow keys, numpad)
 *  - mousemask (cursor)
 *  - getmouse
 */

#ifndef CURSES_H_
#define CURSES_H_ 1

#include <stdarg.h>
#include <stdbool.h>
#include <kernel/color.h>
#include <kernel/framebuffer_console.h>

#undef ERR
#define ERR (-1)

#undef OK
#define OK (0)

/**
 * @def CURSES_MAX_PAIRS
 * @brief Maximum color pairs that a WINDOW can support after calling
 * start_color()
 */
#define CURSES_MAX_PAIRS 16

/**
 * @struct WINDOW
 * @brief Curses window structure.
 * @defails Old and new framebuffer contexts + color pairs initialized by the
 * user.
 */
typedef struct {
    fbc_ctx* old_ctx;  /**< @brief Last ctx */
    fbc_ctx* ctx;      /**< @brief Framebuffer console context */
    color_pair* pairs; /**< @brief Array of color pairs */
} WINDOW;

#if defined(_IN_CURSES_LIB) /* We included from curses.c */
/**
 * @var stdscr
 * @brief Standard curses WINDOW.
 * @details Declared extern when including the curses header; declared and
 * initialized when compiling the curses lib source. That way there is only 1
 * real stdscr per curses project.
 */
WINDOW* stdscr = NULL;

/**
 * @var COLOR_PAIRS
 * @brief Number of supported color pairs in the current WINDOW.
 * @details Initialized at runtime by start_color(). Same declaration method as
 * stdscr.
 */
uint16_t COLOR_PAIRS = 0;
#else  /* We included from other source */
extern WINDOW* stdscr;
extern int COLOR_PAIRS;
#endif /* _IN_CURSES_LIB */

/**
 * @brief Allocate and fill a new curses window struct, and return it.
 * @details It will use stdscr the first time.
 * @return Allocated WINDOW.
 */
WINDOW* initscr(void);

/**
 * @brief End curses window.
 * @details Switch to the old fbc context and free the allocated window.
 * @return OK if success, ERR otherwise.
 */
int endwin(void);

/**
 * @brief disable keyboard line buffer (getchar() returns user input
 * inmediately).
 * @return OK if success, ERR otherwise.
 */
int raw(void);

/**
 * @brief Enable keyboard line buffer (getchar returns user input on newline).
 * @return OK if success, ERR otherwise.
 */
int noraw(void);

/**
 * @brief Prints characters as the user is typing them.
 * @return OK if success, ERR otherwise.
 */
int echo(void);

/**
 * @brief Disables character printing when the user is typing.
 * @return OK if success, ERR otherwise.
 */
int noecho(void);

/**
 * @brief refreshes the current WINDOW (fbc context).
 * @return OK if success, ERR otherwise.
 */
int refresh(void);

/**
 * @brief Refreshes the specified window
 * @param[out] win Window to be refreshed.
 * @return OK if success, ERR otherwise.
 */
int wrefresh(WINDOW* win);

/**
 * @brief Change cursor position of the current window.
 * @param[in] y, x New position on the console.
 * @return OK if success, ERR otherwise.
 */
int move(uint32_t y, uint32_t x);

/**
 * @brief Change cursor position of the specified window
 * @param[out] win Window for moving the cursor.
 * @param[in] y, x New position on the console.
 * @return OK if success, ERR otherwise.
 */
int wmove(WINDOW* win, uint32_t y, uint32_t x);

/**
 * @def getyx
 * @brief Curses macro to avoid `&`
 */
#define getyx(win, y, x) _getyx(win, &y, &x)

/**
 * @brief Write the cursor position of the specified window to the y and x
 * pointers.
 * @param[in] win Window for getting the cursor.
 * @param[out] y Pointer for the Y coordinate.
 * @param[out] x Pointer for the X coordinate.
 */
void _getyx(WINDOW* win, int* y, int* x);

/**
 * @brief Prints with format `fmt` in the current window.
 * @param[in] fmt Format string.
 * @return Bytes written.
 */
int printw(const char* fmt, ...);

/**
 * @brief Prints with format `fmt` using a va_list
 * @param[in] fmt Format string.
 * @param[in] va Variable argument list.
 * @return Bytes written.
 */
int vprintw(const char* fmt, va_list va);

/**
 * @brief Move to `(y, x)` and print with format `fmt`
 * @param[in] y, x Position to move.
 * @param[in] fmt Format string.
 * @return Bytes written.
 */
int mvprintw(int y, int x, const char* fmt, ...);

/**
 * @brief Print the specified character.
 * @param[in] ch Character to print.
 * @return Character printed.
 */
int addch(int ch);

/**
 * @brief Move to `(y,x)` and print the specified character.
 * @param[in] y, x Position to move
 * @param[in] ch Character to print
 * @return OK if success, ERR otherwise.
 */
int mvaddch(int y, int x, int ch);

/**
 * @brief Clear to end of line.
 * @return OK if success, ERR otherwise.
 */
int clrtoeol(void);

/**
 * @brief Clear to end of line in the specified window.
 * @param[out] win Window for clearing the current line.
 * @return OK if success, ERR otherwise.
 */
int wclrtoeol(WINDOW* win);

/**
 * @brief Get character from user input.
 * @return OK if success, ERR otherwise.
 */
int getch(void);

/**
 * @brief Clear the current window
 * @return OK if success, ERR otherwise.
 */
int clear(void);

/**
 * @brief Clear the specified window.
 * @param[out] win Window to clear.
 * @return OK if success, ERR otherwise.
 */
int wclear(WINDOW* win);

/**
 * @brief Returns true if the terminal supports color.
 * @details For now useless, always returns true.
 * @return True if the program supports colors.
 */
bool has_colors(void);

/**
 * @brief Start color mode in the current window.
 * @return OK if success, ERR otherwise.
 */
int start_color(void);

/**
 * @brief Assigns the specifed foreground and background to the specified
 * pair index.
 * @details start_color() needs to be called first.
 * @param[in] pair Identificator for the new pair.
 * @param[in] fg 32 bit foreground color for the pair.
 * @param[in] bg 32 bit background color for the pair.
 * @return OK if success, ERR otherwise.
 */
int init_pair(uint16_t pair, uint32_t fg, uint32_t bg);

/**
 * @brief Changes the current terminal colors to the fg and bg of the
 * specified pair index.
 * @details This function will not check if the color pair has been
 * Initialized, and start_color() needs to be called first.
 * @param[in] pair Color pair index.
 * @return OK if success, ERR otherwise.
 */
int use_pair(uint16_t pair);

/**
 * @brief Reset to the default terminal colors.
 * @return OK if success, ERR otherwise.
 */
int reset_pair(void);

/**
 * @brief Toggles the foreground/background order in the specified pair,
 * overwriting it.
 * @param[in] pair Pair index to be inverted.
 * @return OK if success, ERR otherwise.
 */
int invert_pair(uint16_t pair);

#endif /* CURSES_H_ */
