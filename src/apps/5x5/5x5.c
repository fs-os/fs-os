/**
 * @file      5x5.c
 * @brief     Main file for the 5x5 game.
 * @author    8dcc
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> /* tolower */
#include <time.h>
#include <curses.h>

#include "defines.h"

/**
 * @var use_color
 * @brief Used to check if we can use color at runtime
 */
static bool use_color = false;

/*-----------------------------------------------------------------------------*/

/**
 * @brief Parses a pair string with format `AxB` using atoi.
 * @param[out] a Pointer where to save the first value of the pair (usually
 * width)
 * @param[out] b Pointer where to save the second value of the pair (usually
 * height)
 * @param[inout] src String containing the pair in `AxB` format.
 */
static inline void parse_pair(uint16_t* a, uint16_t* b, char* src) {
    *a = 0;
    *b = 0;

    char* start = src;
    while (*src != 'x' && *src != '\0')
        src++;

    /* No x, invalid format */
    if (*src != 'x')
        return;

    /* Cut string at 'x', make it point to start of 2nd digit */
    *src++ = '\0';

    *a = atoi(start);
    *b = atoi(src);
}

/**
 * @brief Parses the program arguments changing the settings of the current
 * game.
 * @param[in] argc Number of arguments from main.
 * @param[in] argv String vector with the artuments.
 * @param[out] ctx Target ctx_t struct to write the changes.
 * @return False if the caller (main()) needs to exit. True otherwise.
 */
static inline bool parse_args(int argc, char** argv, ctx_t* ctx) {
    bool arg_error = false;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--resolution")) {
            if (i == argc - 1) {
                printf("Not enough arguments for \"%s\"\n", argv[i]);
                arg_error = true;
                break;
            }

            i++;
            parse_pair(&ctx->w, &ctx->h, argv[i]);
            if (ctx->w < MIN_W || ctx->h < MIN_H) {
                printf("Invalid resolution format for \"%s\".\n"
                       "Minimum resolution: %dx%d\n",
                       argv[i - 1], MIN_W, MIN_H);
                arg_error = true;
                break;
            }
        } else if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--scale")) {
            if (i == argc - 1) {
                printf("Not enough arguments for \"%s\"\n", argv[i]);
                arg_error = true;
                break;
            }

            i++;
            parse_pair(&ctx->ysc, &ctx->xsc, argv[i]);
            if (ctx->ysc < 1 || ctx->xsc < 1) {
                printf("Invalid scale format for \"%s\".\n"
                       "Minimum scale: 1\n",
                       argv[i - 1]);
                arg_error = true;
                break;
            }
        } else if (!strcmp(argv[i], "-k") || !strcmp(argv[i], "--keys")) {
            puts("Controls:\n"

                 "        hjkl - Move in the grid (vim-like)\n"
                 "     <space> - Toggle selected cell (and adjacent)\n"
                 "           r - Generate random grid\n"
#ifdef CHEAT
                 "           c - Generate a cheat grid\n"
#endif
                 "           q - Quit the game");
            return false;
        } else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            arg_error = true;
            break;
        }
    }

    if (arg_error) {
        printf("Usage:\n"
               "    %s                   - Launch with default resolution and "
               "scale\n"
               "    %s --help            - Show this help\n"
               "    %s -h                - Same as --help\n"
               "    %s --keys            - Show the controls\n"
               "    %s -k                - Same as --keys\n"
               "    %s --resolution WxH  - Launch with specified resolution "
               "(width, height)\n"
               "    %s -r WxH            - Same as --resolution\n"
               "    %s --scale VxH       - Launch with specified scale "
               "(vertical, horizontal)\n"
               "    %s -s VxH            - Same as --scale\n",
               argv[0], argv[0], argv[0], argv[0], argv[0], argv[0], argv[0],
               argv[0], argv[0]);
        return false;
    }

    return true;
}

/**
 * @brief Initialize the game grid.
 * @param[out] ctx Game context used to write the new grid.
 * @param[in] random Will fill it with ON cells in random positions if true, or
 * all OFF otherwise.
 */
static inline void init_grid(ctx_t* ctx, bool random) {
    for (int y = 0; y < ctx->h; y++) {
        for (int x = 0; x < ctx->w; x++) {
            /* There is 50% chance a tile is turned on if random == true */
            if (random && rand() % 2)
                ctx->grid[y * ctx->w + x] = ON_CH;
            else
                ctx->grid[y * ctx->w + x] = OFF_CH;
        }
    }
}

/**
 * @brief Prints a message under the game grid.
 * @details Doesn't change the cursor position.
 * @param[in] s String to display.
 */
static inline void print_message(ctx_t* ctx, const char* s) {
    int oy, ox;
    getyx(stdscr, oy, ox);

    mvprintw((ctx->h * ctx->ysc) + 3, 1, s);

    move(oy, ox);
}

/**
 * @brief Clears a line in the screen.
 * @details Doesn't change the cursor position.
 * @param[in] y Line number starting from 0 to clear.
 */
static inline void clear_line(int y) {
    int oy, ox;
    getyx(stdscr, oy, ox);

    move(y, 0);
    clrtoeol();

    move(oy, ox);
}

/**
 * @brief Draws the grid border for the game.
 * @param[in] ctx Game context structure for the width and height.
 */
static inline void draw_border(const ctx_t* ctx) {
    const int xspacing = (ctx->xsc >= 3) ? 1 : 0;
    const int real_w   = ctx->w * (ctx->xsc + xspacing) + xspacing;
    const int real_h   = ctx->h * ctx->ysc;

    SET_COL(COL_GRAY);

    /* First line */
    mvaddch(0, 0, '+');
    for (int x = 0; x < real_w; x++)
        mvaddch(0, x + 1, '-');
    mvaddch(0, real_w + 1, '+');

    /* Mid lines */
    for (int y = 1; y <= real_h; y++) {
        mvaddch(y, 0, '|');
        mvaddch(y, real_w + 1, '|');
    }

    /* Last line */
    mvaddch(real_h + 1, 0, '+');
    for (int x = 0; x < real_w; x++)
        mvaddch(real_h + 1, x + 1, '-');
    mvaddch(real_h + 1, real_w + 1, '+');

    RESET_COL();
}

/**
 * @brief Redraws the grid based on the ctx_t.grid array.
 * @details The cursor in the context struct is the position inside the
 * (unscaled) grid. This function will use it to move to the real post-scale
 * cursor position on the terminal.
 *
 * @param[in] ctx Game context structure for the grid.
 */
static void redraw_grid(const ctx_t* ctx) {
    const int xspacing = (ctx->xsc >= 3) ? 1 : 0;
    const int yborder  = 1;
    const int xborder  = 1 + xspacing;
    const int ysc      = ctx->ysc;
    const int xsc      = ctx->xsc + xspacing;

    draw_border(ctx);

    /* Update the cursor to the real position:
     *   - In the case of the X coor, add 1 to scale for extra horizontal
     *     spacing (plus 1 to horizontal border).
     *   - Get scaled position of tile.
     *   - Go to center of the tile (Using scale without spacing).
     *   - Add border size to get real position.
     *   - Subtract 1 to get the zero-starting index. */
    const int cursor_y =
      (ctx->cursor.y * ysc) + (ctx->ysc - ctx->ysc / 2) + yborder - 1;
    const int cursor_x =
      (ctx->cursor.x * xsc) + (ctx->xsc - ctx->xsc / 2) + xborder - 1;

    /* Iterate tiles from grid */
    for (int y = 0; y < ctx->h; y++) {
        for (int x = 0; x < ctx->w; x++) {
            const char c = ctx->grid[y * ctx->w + x];

            /* Draw the actual scaled tile in the real positions. Add real scale
             * without spacing in the loops so we don't fill the spacing */
            const int term_y = y * ysc + yborder;
            const int term_x = x * xsc + xborder;
            for (int ty = term_y; ty < term_y + ctx->ysc; ty++)
                for (int tx = term_x; tx < term_x + ctx->xsc; tx++) {
                    if (ty == cursor_y && tx == cursor_x)
                        INVERT_COL(COL_WHITE);

                    SET_COL(COL_WHITE);
                    mvaddch(ty, tx, c);
                    RESET_COL();

                    if (ty == cursor_y && tx == cursor_x)
                        INVERT_COL(COL_WHITE);
                }
        }
    }

    move(cursor_y, cursor_x);
}

/**
 * @brief Toggle the selected grid cell, and the adjacent ones.
 * @details Adjacent meaning up, down, left and right.
 * @param[inout] ctx Context struct used for the cursor and the grid.
 */
static inline void toggle_adjacent(ctx_t* ctx) {
    const int y = ctx->cursor.y;
    const int x = ctx->cursor.x;

    /* Key actual position */
    if (ctx->grid[y * ctx->w + x] == ON_CH)
        ctx->grid[y * ctx->w + x] = OFF_CH;
    else
        ctx->grid[y * ctx->w + x] = ON_CH;

    /* Up */
    if (y > 0) {
        if (ctx->grid[(y - 1) * ctx->w + x] == ON_CH)
            ctx->grid[(y - 1) * ctx->w + x] = OFF_CH;
        else
            ctx->grid[(y - 1) * ctx->w + x] = ON_CH;
    }

    /* Down */
    if (y < ctx->h - 1) {
        if (ctx->grid[(y + 1) * ctx->w + x] == ON_CH)
            ctx->grid[(y + 1) * ctx->w + x] = OFF_CH;
        else
            ctx->grid[(y + 1) * ctx->w + x] = ON_CH;
    }

    /* Left */
    if (x > 0) {
        if (ctx->grid[y * ctx->w + (x - 1)] == ON_CH)
            ctx->grid[y * ctx->w + (x - 1)] = OFF_CH;
        else
            ctx->grid[y * ctx->w + (x - 1)] = ON_CH;
    }

    /* Right */
    if (x < ctx->w - 1) {
        if (ctx->grid[y * ctx->w + (x + 1)] == ON_CH)
            ctx->grid[y * ctx->w + (x + 1)] = OFF_CH;
        else
            ctx->grid[y * ctx->w + (x + 1)] = ON_CH;
    }
}

/**
 * @brief Checks if the user won the game.
 * @param[in] ctx Context pointer used to check if there is an OFF tile.
 * @return True if all tiles are ON.
 */
static inline bool check_win(const ctx_t* ctx) {
    for (int y = 0; y < ctx->h; y++)
        for (int x = 0; x < ctx->w; x++)
            if (ctx->grid[y * ctx->w + x] == OFF_CH)
                return false;

    /* We won! */
    return true;
}

int main_5x5(int argc, char** argv) {
    /* Main context struct */
    ctx_t ctx = (ctx_t){
        .h      = DEFAULT_H,
        .w      = DEFAULT_W,
        .ysc    = DEFAULT_YSC,
        .xsc    = DEFAULT_XSC,
        .cursor = (point_t){ 0, 0 },
        .grid   = NULL,
    };

    /* Parse arguments before ncurses */
    if (!parse_args(argc, argv, &ctx))
        return 1;

    /* Real cursor position after parsing args */
    ctx.cursor = (point_t){
        ctx.h - ctx.h / 2 - 1,
        ctx.w - ctx.w / 2 - 1,
    };

    initscr(); /* Init ncurses */
    raw();     /* Scan input without pressing enter */
    noecho();  /* Don't print when typing */

#ifdef USE_COLOR
    /* Global used to indicate redraw_grid that color is supported at runtime */
    use_color = has_colors();

    if (use_color) {
        start_color();

        init_pair(COL_GRAY, COLOR_GRAY, COLOR_BLACK);
        init_pair(COL_WHITE, COLOR_WHITE_B, COLOR_BLACK);
    }
#endif

    /* Init random seed */
    srand(time(NULL));

    /* Allocate and initialize grid */
    ctx.grid = calloc(ctx.w * ctx.h, sizeof(uint8_t));
    init_grid(&ctx, false);

    /* Char the user is pressing */
    int c = 0;
    do {
        /* First, redraw the grid */
        redraw_grid(&ctx);

        /* Refresh screen */
        refresh();

        /* Wait for user input */
        c = tolower(getch());

        /* Clear the output line */
        clear_line((ctx.h * ctx.ysc) + 3);

        /* Parse input. 'q' quits and there is vim-like navigation */
        switch (c) {
            case 'k':
                if (ctx.cursor.y > 0)
                    ctx.cursor.y--;
                break;
            case 'j':
                if (ctx.cursor.y < ctx.h - 1)
                    ctx.cursor.y++;
                break;
            case 'h':
                if (ctx.cursor.x > 0)
                    ctx.cursor.x--;
                break;
            case 'l':
                if (ctx.cursor.x < ctx.w - 1)
                    ctx.cursor.x++;
                break;
            case ' ':
                toggle_adjacent(&ctx);

                if (check_win(&ctx))
                    print_message(&ctx, "You won!");

                break;
            case 'r':
                /* Generate again but in random positions */
                init_grid(&ctx, true);
                break;
#ifdef CHEAT
            case 'c':
                print_message(&ctx, "Generating cheat state...");
                char arr[] = "#####"
                             "##.##"
                             "#...#"
                             "##.##"
                             "#####";
                memcpy(ctx.grid, arr, sizeof(arr) - 1);
                break;
#endif
            case KEY_CTRLC:
                c = 'q';
                break;
            case 'q':
            default:
                break;
        }
    } while (c != 'q');

    endwin();

    return 0;
}
