
#ifndef _CURSES_H
#define _CURSES_H 1

#include <stdarg.h>
#include <kernel/framebuffer_console.h>

typedef struct {
    fbc_ctx* old_ctx; /* Last ctx */
    fbc_ctx* ctx;     /* Framebuffer console context */
} WINDOW;

#if defined(_IN_CURSES_LIB) /* We included from curses.c */
extern WINDOW* stdscr;
extern int COLOR_PAIRS;
#elif !defined(_HAS_CURSES_GLOBALS) /* We included from other source */
#define _HAS_CURSES_GLOBALS 1
WINDOW* stdscr  = NULL;
int COLOR_PAIRS = 0;
#endif /* _IN_CURSES_LIB */

/*
 * DONE:
 *  - initscr
 *  - endwin
 *  - raw       (kb_raw)
 *  - noraw     (kb_noraw)
 *  - echo      (kb_echo)
 *  - noecho    (kb_noecho)
 *  - refresh   (fbc_refresh)
 *  - wrefresh  (switch ctx & fbc_refresh)
 *  - move
 *  - wmove     (window move)
 *  - getyx
 *  - printw    (printf)
 *  - vprintw   (vprintf)
 *  - mvprintw  (move (no call) & printf)
 *  - addch     (putchar)
 *  - mvaddch   (move (no call) & putchar)
 *  - clrtoeol  (clear to end of line)
 *  - wclrtoeol (window clear to end of line)
 *  - getch     (getchar)
 *  - clear     (fbc_clear)
 *  - wclear    (window clear)
 *
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
 *  - has_colors
 *  - start_color
 *  - init_pair
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

#endif /* _CURSES_H */

