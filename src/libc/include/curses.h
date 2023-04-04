
#ifndef _CURSES_H
#define _CURSES_H 1

#include <stdarg.h>
#include <stdbool.h>
#include <kernel/color.h>
#include <kernel/framebuffer_console.h>

#define CURSES_MAX_PAIRS 16

typedef struct {
    fbc_ctx* old_ctx;  /* Last ctx */
    fbc_ctx* ctx;      /* Framebuffer console context */
    color_pair* pairs; /* Array of color pairs */
} WINDOW;

#if defined(_IN_CURSES_LIB) /* We included from curses.c */
WINDOW* stdscr       = NULL;
uint16_t COLOR_PAIRS = 0;
#else  /* We included from other source */
extern WINDOW* stdscr;
extern int COLOR_PAIRS;
#endif /* _IN_CURSES_LIB */

/*
 * TODO:
 *  - wprintw   (window printf)
 *  - mvwprintw (move & window printf)
 *  - waddch    (window putchar)
 *  - mvwaddch  (move & window putchar)
 *
 * ERROR:
 *  - keypad    (arrow keys, numpad)
 *  - mousemask (cursor)
 *  - getmouse
 */

/* initsrc: allocate and fill a new curses window, and returns it. It will use
 * stdscr the first time */
WINDOW* initscr(void);

/* endwin: switch to the old fbc context and free the allocated window */
int endwin(void);

/* raw: disable keyboard line buffer (getchar returns user input inmediately) */
int raw(void);

/* noraw: enable keyboard line buffer (getchar returns user input on newline) */
int noraw(void);

/* echo: prints characters as the user is typing them */
int echo(void);

/* noecho: disables character printing when the user is typing */
int noecho(void);

/* refresh: refreshes the current window (fbc context) */
int refresh(void);

/* wrefresh: refreshes the specified window */
int wrefresh(WINDOW* win);

/* move: change cursor position of the current window */
int move(int y, int x);

/* wmove: change cursor position of the specified window */
int wmove(WINDOW* win, int y, int x);

/* getyx: ncurses macro to avoid & */
#define getyx(win, y, x) _getyx(win, &y, &x)

/* _getyx: write the cursor position of the specified window to the y and x
 * pointers */
void _getyx(WINDOW* win, int* y, int* x);

/* printw: prints with format "fmt" */
int printw(const char* fmt, ...);

/* vprintw: prints with format "fmt" using a va list */
int vprintw(const char* fmt, va_list va);

/* mvprintw: move to (y,x) and print with format "fmt" */
int mvprintw(int y, int x, const char* fmt, ...);

/* addch: print the specified character */
int addch(int ch);

/* mvaddch: move to (y,x) and print the specified character */
int mvaddch(int y, int x, int ch);

/* clrtoeol: clear to end of line */
int clrtoeol(void);

/* wclrtoeol: clear to end of line in the specified window */
int wclrtoeol(WINDOW* win);

/* getch: get input character */
int getch(void);

/* clear: clear the current window */
int clear(void);

/* wclear: clear the specified window */
int wclear(WINDOW* win);

/* has_colors: returns true if the terminal supports color. Useless */
bool has_colors(void);

/* start_color: start color mode in the current window */
int start_color(void);

/* init_pair: assigns the specifed foreground and background to the specified
 * pair index. start_color needs to be called first */
int init_pair(uint16_t pair, uint32_t fg, uint32_t bg);

/* use_pair: changes the current terminal colors to the fg and bg of the
 * specified pair index. This function will not check if the color pair has been
 * Initialized, and start_color needs to be called first */
int use_pair(uint16_t pair);

/* reset_pair: reset to the default terminal colors */
int reset_pair(void);

/* invert_pair: toggles the foreground/background order in the specified pair */
int invert_pair(uint16_t pair);

#endif /* _CURSES_H */

