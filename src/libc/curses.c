
#include <curses.h>
#include <stdlib.h>
#include <kernel/framebuffer_console.h>
#include <kernel/color.h>

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
    fbc_change_ctx(win->old_ctx);
    fbc_refresh();

    free(win->ctx->fbc); /* Free the framebuffer console array */
    free(win->ctx);      /* Free the framebuffer console context */
    free(win);           /* Free the curses window struct */

    return 0;
}

