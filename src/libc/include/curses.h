
#ifndef _CURSES_H
#define _CURSES_H 1

#include <kernel/framebuffer_console.h>

typedef struct {
    fbc_ctx* old_ctx; /* Last ctx */
    fbc_ctx* ctx;     /* Framebuffer console context */
} WINDOW;

/* initsrc: allocate and fill a new curses window, and return it */
WINDOW* initscr(void);

/* endwin: switch to the old fbc context and free the allocated window */
int endwin(WINDOW* win);

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
 *
 * TODO:
 *  - move
 *  - getyx     (2 arg version, not a macro)
 *  - printw    (printf)
 *  - mvprintw  (move (no call) & printf)
 *  - vprintw   (vprintf)
 *  - addch     (putchar)
 *  - mvaddch   (move (no call) & putchar)
 *  - clrtoeol  (clear to end of line)
 *  - getch     (getchar)
 *
 * ERROR:
 *  - wprintw   (window printf)
 *  - mvwprintw (move & window printf)
 *  - waddch    (window putchar)
 *  - mvwaddch  (move & window putchar)
 *  - keypad    (arrow keys, numpad)
 *  - mousemask (cursor)
 *  - getmouse
 *  - has_colors
 *  - start_color
 *  - init_pair
 */

#endif /* _CURSES_H */

