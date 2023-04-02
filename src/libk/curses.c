
#include <curses.h>
#include <stdlib.h>
#include <kernel/color.h>
#include <kernel/framebuffer_console.h>
#include <kernel/keyboard.h>

#define DEFAULT_FG COLOR_WHITE
#define DEFAULT_BG COLOR_BLACK

/* initsrc: allocate and fill a new curses window, and return it */
WINDOW* initscr(void) {
    fbc_ctx* cur = fbc_get_ctx();
    WINDOW* win  = malloc(sizeof(WINDOW));

    /* Fill the curses window with the new and old contexts */
    win->old_ctx = cur;
    win->ctx     = malloc(sizeof(fbc_ctx));

    /* Fill the new framebuffer console context */
    win->ctx->fbc = malloc(cur->ch_h * cur->ch_w * sizeof(fbc_entry));

    win->ctx->y = cur->y;
    win->ctx->x = cur->x;
    win->ctx->h = cur->h;
    win->ctx->w = cur->w;

    win->ctx->ch_h = cur->ch_h;
    win->ctx->ch_w = cur->ch_w;

    win->ctx->font = cur->font;

    win->ctx->cur_cols.fg = DEFAULT_FG;
    win->ctx->cur_cols.bg = DEFAULT_BG;

    win->ctx->cur_y = 0;
    win->ctx->cur_x = 0;

    /* Switch to the new fbc context */
    fbc_change_ctx(win->ctx);
    fbc_clear();
    fbc_refresh();

    return win;
}

/* endwin: switch to the old fbc context and free the allocated window */
int endwin(WINDOW* win) {
    /* Switch to old ctx and refresh the screen */
    fbc_change_ctx(win->old_ctx);
    fbc_refresh();

    /* Reset keyboard modes in case they were changed. For now it works until we
     * can have process-specific settings for keyboard, terminal, etc. */
    kb_echo();
    kb_noraw();

    free(win->ctx->fbc); /* Free the framebuffer console array */
    free(win->ctx);      /* Free the framebuffer console context */
    free(win);           /* Free the curses window struct */

    return 0;
}

/* raw: disable keyboard line buffer (getchar returns user input inmediately) */
int raw(void) {
    kb_raw();
    return 0;
}

/* noraw: enable keyboard line buffer (getchar returns user input on newline) */
int noraw(void) {
    kb_noraw();
    return 0;
}

/* echo: prints characters as the user is typing them */
int echo(void) {
    kb_echo();
    return 0;
}

/* noecho: disables character printing when the user is typing */
int noecho(void) {
    kb_noecho();
    return 0;
}

/* refresh: refreshes the current window (fbc context) */
int refresh(void) {
    fbc_refresh();
    return 0;
}

/* wrefresh: refreshes the specified window */
int wrefresh(WINDOW* win) {
    fbc_ctx* old_ctx = fbc_get_ctx();

    fbc_change_ctx(win->ctx);
    fbc_refresh();

    fbc_change_ctx(old_ctx);
    return 0;
}

