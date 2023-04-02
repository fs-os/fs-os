
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

/*
 * DONE:
 *  - initscr
 *  - endwin
 *
 * TODO:
 *  - raw       (add kb_raw)
 *  - noecho    (kb_noecho)
 *  - refresh   (fbc_refresh)
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

