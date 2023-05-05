
/* Used to check if we are declaring globals extern */
#define _IN_CURSES_LIB 1

#include <curses.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <kernel/color.h>
#include <kernel/framebuffer_console.h>
#include <kernel/keyboard.h>

WINDOW* initscr(void) {
    fbc_ctx* cur = fbc_get_ctx();
    WINDOW* win  = malloc(sizeof(WINDOW));

    /* If we didn't initialize stdscr, use it */
    if (stdscr == NULL)
        stdscr = win;

    /* Fill the curses window with the new and old contexts */
    win->old_ctx = cur;
    win->ctx     = malloc(sizeof(fbc_ctx));
    win->pairs   = NULL; /* Initialized by start_color */

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
    fbc_refresh_raw();

    return win;
}

int endwin(void) {
    /* Switch to old ctx and refresh the screen. Unlike in initscr, we are not
     * clearing so we don't use fbc_refresh_raw() */
    fbc_change_ctx(stdscr->old_ctx);
    fbc_refresh();

    /* Reset keyboard modes in case they were changed. For now it works until we
     * can have process-specific settings for keyboard, terminal, etc. */
    kb_echo();
    kb_noraw();

    /* We called start_color, free the allocated array */
    if (COLOR_PAIRS > 0)
        free(stdscr->pairs);

    free(stdscr->ctx->fbc); /* Free the framebuffer console array */
    free(stdscr->ctx);      /* Free the framebuffer console context */
    free(stdscr);           /* Free the curses window struct */

    /* So next call to initscr uses stdscr */
    stdscr = NULL;

    return OK;
}

int raw(void) {
    kb_raw();
    return OK;
}

int noraw(void) {
    kb_noraw();
    return OK;
}

int echo(void) {
    kb_echo();
    return OK;
}

int noecho(void) {
    kb_noecho();
    return OK;
}

int refresh(void) {
    fbc_refresh_raw();
    return OK;
}

int wrefresh(WINDOW* win) {
    fbc_ctx* old_ctx = fbc_get_ctx();

    fbc_change_ctx(win->ctx);
    fbc_refresh_raw();

    fbc_change_ctx(old_ctx);
    return OK;
}

int move(uint32_t y, uint32_t x) {
    fbc_ctx* ctx = fbc_get_ctx();

    if (y >= ctx->ch_h)
        y = ctx->ch_h - 1;

    if (x >= ctx->ch_w)
        x = ctx->ch_w - 1;

    ctx->cur_y = y;
    ctx->cur_x = x;

    return OK;
}

int wmove(WINDOW* win, uint32_t y, uint32_t x) {
    if (y >= win->ctx->ch_h)
        y = win->ctx->ch_h - 1;

    if (x >= win->ctx->ch_w)
        x = win->ctx->ch_w - 1;

    win->ctx->cur_y = y;
    win->ctx->cur_x = x;

    return OK;
}

void _getyx(WINDOW* win, int* y, int* x) {
    *y = win->ctx->cur_y;
    *x = win->ctx->cur_x;
}

int printw(const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);

    /* Just call vprintf instead of vprintw */
    int ret = vprintf(fmt, va);

    va_end(va);
    return ret;
}

int vprintw(const char* fmt, va_list va) {
    return vprintf(fmt, va);
}

int mvprintw(int y, int x, const char* fmt, ...) {
    move(y, x);

    va_list va;
    va_start(va, fmt);

    /* Just call vprintf instead of vprintw */
    int ret = vprintf(fmt, va);

    va_end(va);
    return ret;
}

int addch(int ch) {
    return putchar(ch);
}

int mvaddch(int y, int x, int ch) {
    move(y, x);
    return putchar(ch);
}

int clrtoeol(void) {
    fbc_clrtoeol();
    return OK;
}

int wclrtoeol(WINDOW* win) {
    fbc_ctx* old_ctx = fbc_get_ctx();

    fbc_change_ctx(win->ctx);
    fbc_clrtoeol();

    fbc_change_ctx(old_ctx);
    return OK;
}

int getch(void) {
    return getchar();
}

int clear(void) {
    fbc_clear();
    fbc_refresh_raw();
    return OK;
}

int wclear(WINDOW* win) {
    fbc_ctx* old_ctx = fbc_get_ctx();

    fbc_change_ctx(win->ctx);
    fbc_clear();
    fbc_refresh_raw();

    fbc_change_ctx(old_ctx);
    return OK;
}

bool has_colors(void) {
    /* Useless, fbc supports color */
    return true;
}

int start_color(void) {
    /* Allocate the color pairs array */
    stdscr->pairs = calloc(CURSES_MAX_PAIRS, sizeof(color_pair));

    COLOR_PAIRS = CURSES_MAX_PAIRS;
    return OK;
}

int init_pair(uint16_t pair, uint32_t fg, uint32_t bg) {
    /* Pair index out of bounds */
    if (pair >= COLOR_PAIRS)
        return ERR;

    stdscr->pairs[pair].fg = fg;
    stdscr->pairs[pair].bg = bg;

    return OK;
}

int use_pair(uint16_t pair) {
    /* Pair index out of bounds */
    if (pair >= COLOR_PAIRS)
        return ERR;

    fbc_setcol(stdscr->pairs[pair].fg, stdscr->pairs[pair].bg);

    return OK;
}

int reset_pair(void) {
    fbc_setcol(DEFAULT_FG, DEFAULT_BG);
    return OK;
}

int invert_pair(uint16_t pair) {
    /* Pair index out of bounds */
    if (pair >= COLOR_PAIRS)
        return ERR;

    /* Swap */
    const uint32_t tmp     = stdscr->pairs[pair].fg;
    stdscr->pairs[pair].fg = stdscr->pairs[pair].bg;
    stdscr->pairs[pair].bg = tmp;

    return OK;
}
