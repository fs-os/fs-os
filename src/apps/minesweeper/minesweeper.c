
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>  /* time */
#include <ctype.h> /* tolower, isdigit */
#include <curses.h>

#include "defines.h"

#define DIFFIC2BOMBPERCENT(d) ((MAX_BOMBS - MIN_BOMBS) * d / 100 + MIN_BOMBS)

/**
 * @struct Tile
 * @brief Tile of the minesweeper.
 * @description The char indicates its contents and the flags special
 * information (for example if the user flagged the tile, revealed it...)
 */
typedef struct {
    char c;        /* Char in that tile, actual item */
    uint8_t flags; /* Tile status (revealed, flagged, etc) */
} Tile;

/**
 * @struct Game
 * @brief Minesweeper struct containing the game information.
 */
typedef struct {
    uint16_t w, h;      /* Width and height */
    Tile* grid;         /* Pointer to the minesweeper grid */
    uint8_t playing;    /* The user revealed the first tile */
    uint8_t difficulty; /* Percentage of bombs to fill in the grid */
} Game;

/**
 * @struct vec2_t
 * @brief Two-dimensional vector
 */
typedef struct {
    uint16_t x, y; /**< Description */
} vec2_t;

/*----------------------------------------------------------------------------*/

/**
 * @var ms
 * @brief Global game struct
 */
static Game ms;

/**
 * @var use_color
 * @brief Used to check if we can use color at runtime
 */
static bool use_color = false;

/*----------------------------------------------------------------------------*/

/**
 * @brief Parses a resolution string with format `WIDTHxHEIGHT` using atoi.
 * @param[out] dst_w Pointer where to save the resolution's width
 * @param[out] dst_h Pointer where to save the resolution's height
 * @param[inout] src String containing the resolution in `WIDTHxHEIGHT` format
 * @return True on success, false if invalid format
 */
static bool parse_resolution(uint16_t* dst_w, uint16_t* dst_h, char* src) {
    *dst_w = 0;
    *dst_h = 0;

    char* start = src;
    while (isdigit(*src))
        src++;

    /* First non-digit char was not 'x', invalid format */
    if (*src != 'x')
        return false;

    /* Cut string at 'x', make it point to start of 2nd digit */
    *src++ = '\0';

    *dst_w = atoi(start);
    *dst_h = atoi(src);

    return true;
}

/**
 * @brief Parses the program arguments changing the properties of ms.
 * @param[in] argc Number of arguments from main.
 * @param[in] argv String vector with the artuments.
 * @return False if the caller (main()) needs to exit. True otherwise.
 */
static inline bool parse_args(int argc, char** argv) {
    bool arg_error = false;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--resolution")) {
            if (i == argc - 1) {
                printf("Not enough arguments for \"%s\"\n", argv[i]);
                arg_error = true;
                break;
            }

            i++;
            if (!parse_resolution(&ms.w, &ms.h, argv[i]) || ms.w < MIN_W ||
                ms.h < MIN_H) {
                printf("Invalid resolution format for \"%s\".\n"
                       "Minimum resolution: %dx%d\n",
                       argv[i - 1], MIN_W, MIN_H);
                arg_error = true;
                break;
            }
        } else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--difficulty")) {
            if (i == argc - 1) {
                printf("Not enough arguments for \"%s\"\n", argv[i]);
                arg_error = true;
                break;
            }

            ms.difficulty = atoi(argv[++i]);
            if (ms.difficulty < 1 || ms.difficulty > 100) {
                printf("Invalid difficulty format for \"%s\".\n"
                       "Difficulty range: 1-100\n",
                       argv[i - 1]);
                arg_error = true;
                break;
            }
        } else if (!strcmp(argv[i], "-k") || !strcmp(argv[i], "--keys")) {
            printf("Controls:\n"
                   "    <arrows> - Move in the grid (unsupported)\n"
                   "        hjkl - Move in the grid (vim-like)\n"
                   "     <space> - Reveal tile\n"
                   "           f - Flag bomb\n"
#ifdef USE_MOUSE
                   "    <LMouse> - Reveal clicked bomb\n"
                   "    <RMouse> - Flag clicked bomb\n"
#endif
                   "           r - Reveal all tiles and end game\n"
                   "           q - Quit the game\n");
            return 0;
        } else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            arg_error = true;
            break;
        }
    }

    if (arg_error) {
        printf("Usage:\n"
               "    %s                   - Launch with default resolution\n"
               "    %s --help            - Show this help\n"
               "    %s -h                - Same as --help\n"
               "    %s --keys            - Show the controls\n"
               "    %s -k                - Same as --keys\n"
               "    %s --resolution WxH  - Launch with specified resolution "
               "(width, height)\n"
               "    %s -r WxH            - Same as --resolution\n"
               "    %s --difficulty N    - Use specified difficulty from 1 to "
               "100. Default: 40\n"
               "    %s -d N              - Same as --difficulty\n",
               argv[0], argv[0], argv[0], argv[0], argv[0], argv[0], argv[0],
               argv[0], argv[0]);
        return false;
    }

    return true;
}

/**
 * @brief Draws the grid border for the minesweeper.
 */
static void draw_border(void) {
    BOLD_ON();
    SET_COL(COL_NORM);

    /* First line */
    mvaddch(0, 0, '+');
    for (int x = 0; x < ms.w; x++)
        mvaddch(0, x + 1, '-');
    mvaddch(0, ms.w + 1, '+');

    /* Mid lines */
    for (int y = 1; y <= ms.h; y++) {
        mvaddch(y, 0, '|');
        mvaddch(y, ms.w + 1, '|');
    }

    /* Last line */
    mvaddch(ms.h + 1, 0, '+');
    for (int x = 0; x < ms.w; x++)
        mvaddch(ms.h + 1, x + 1, '-');
    mvaddch(ms.h + 1, ms.w + 1, '+');

    RESET_COL();
    BOLD_OFF();
}

/**
 * @brief Initializes the empty background grid for the ms_t struct
 */
static inline void init_grid(void) {
    for (int y = 0; y < ms.h; y++) {
        for (int x = 0; x < ms.w; x++) {
            ms.grid[y * ms.w + x].c     = BACK_CH;
            ms.grid[y * ms.w + x].flags = FLAG_NONE;
        }
    }
}

/**
 * @brief Returns the number of bombs adjacent to a specified tile.
 * @details Adjacent meaning in a 3x3 grid with the speicified tile at its
 * center.
 * @param[in] fy, fx Position of the tile to check.
 * @return Number of bombs adjacent
 */
static int adjacent_bombs(int y, int x) {
    int ret = 0;

    const int start_y = (y > 0) ? y - 1 : y;
    const int start_x = (x > 0) ? x - 1 : x;
    const int end_y   = (y < ms.h - 1) ? y + 1 : y;
    const int end_x   = (x < ms.w - 1) ? x + 1 : x;

    /* ###
     * #X#
     * ### */
    for (int cur_y = start_y; cur_y <= end_y; cur_y++)
        for (int cur_x = start_x; cur_x <= end_x; cur_x++)
            if (ms.grid[cur_y * ms.w + cur_x].c == BOMB_CH)
                ret++;

    return ret;
}

/**
 * @brief Prints the specified message 2 lines bellow `ms`'s grid
 * @param[in] str String to be printed.
 */
static void print_message(const char* str) {
    int y, x;
    getyx(stdscr, y, x);

    SET_COL(COL_NORM);
    mvprintw(ms.h + 3, 1, "%s", str);
    RESET_COL();

    move(y, x);
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
 * @brief Redraws the grid based on the ms.grid array.
 * @details Color macros will only do something if color is enabled and
 * supported.
 * @param[in] cursor Current cursor location for inverting the color.
 */
static void redraw_grid(vec2_t* cursor) {
    const int border_sz = 1;

    draw_border();

    for (int y = 0; y < ms.h; y++) {
        for (int x = 0; x < ms.w; x++) {
            const int final_y = y + border_sz;
            const int final_x = x + border_sz;
            int final_col     = COL_NORM;
            char final_ch     = 0;

            if (ms.grid[y * ms.w + x].flags & FLAG_CLEARED) {
                const int bombs = adjacent_bombs(y, x);
                if (ms.grid[y * ms.w + x].c == BOMB_CH) {
                    /* Bomb (we lost) */
                    final_col = COL_BOMB;
                    final_ch  = BOMB_CH;
                } else if (bombs) {
                    BOLD_ON();
                    /* 5 -> COL_5. See color_ids enum in defines.h */
                    final_col = bombs;

                    /* Number */
                    final_ch = bombs + '0';
                } else {
                    /* Empty tile with no bombs adjacent */
                    final_col = COL_NORM;
                    final_ch  = ms.grid[y * ms.w + x].c;
                }
            } else if (ms.grid[y * ms.w + x].flags & FLAG_FLAGGED) {
                BOLD_ON();
                final_col = COL_FLAG;
                final_ch  = FLAG_CH;
            } else {
                final_col = COL_UNK;
                final_ch  = UNKN_CH;
            }

            if (y == cursor->y && x == cursor->x)
                INVERT_COL(final_col);

            SET_COL(final_col);

            mvaddch(final_y, final_x, final_ch);

            if (y == cursor->y && x == cursor->x)
                INVERT_COL(final_col);

            RESET_COL();
            BOLD_OFF();
        }
    }
}

/**
 * @brief Fill the grid with bombs at random locations.
 * @details Will leave a margin area around the first user selection (so it
 * never reveals a bomb on the first input).
 * @param[in] start Position of the first tile that the user tried to reveal.
 * @param[in] bomb_percent The percentage of bombs to fill.
 */
static void generate_grid(vec2_t start, int bomb_percent) {
    int total_bombs = ms.h * ms.w * bomb_percent / 100;

    /* Actual tiles available for bombs (keep in mind the empty zone around the
     * cursor) */
    const int max_bombs = ms.w * ms.h - BOMB_MARGIN * 4;
    if (total_bombs > max_bombs)
        total_bombs = max_bombs;

    for (int bombs = 0; bombs < total_bombs; bombs++) {
        int bomb_y = (uint32_t)rand() % ms.h;
        int bomb_x = (uint32_t)rand() % ms.w;

        /* Leave an empty zone around cursor */
        if (bomb_y > start.y - BOMB_MARGIN && bomb_y < start.y + BOMB_MARGIN &&
            bomb_x > start.x - BOMB_MARGIN && bomb_x < start.x + BOMB_MARGIN) {
            bombs--;
            continue;
        }

        ms.grid[bomb_y * ms.w + bomb_x].c = BOMB_CH;
    }
}

/**
 * @brief Returns true if all adjacent bombs from a cell have been flagged.
 * @details Assumes adjacent_bombs() has been called and it didn't return 0.
 * @param[in] y, x Position to check
 * @return True if all adjacent bombs have been flagged by the user.
 */
static inline bool surrounding_bombs_flagged(int y, int x) {
    /* NOTE: Assumes adjacent_bombs() has been called and it didn't return 0 */

    const int start_y = (y > 0) ? y - 1 : y;
    const int start_x = (x > 0) ? x - 1 : x;
    const int end_y   = (y < ms.h - 1) ? y + 1 : y;
    const int end_x   = (x < ms.w - 1) ? x + 1 : x;

    for (int cur_y = start_y; cur_y <= end_y; cur_y++)
        for (int cur_x = start_x; cur_x <= end_x; cur_x++)
            /* We found an adjacent bomb and it was not flagged */
            if (ms.grid[cur_y * ms.w + cur_x].c == BOMB_CH &&
                !(ms.grid[cur_y * ms.w + cur_x].flags & FLAG_FLAGGED))
                return false;

    return true;
}

/**
 * @brief Reveals the needed tiles using recursion, starting at y and x.
 * @param[in] y, x Position to be revealed.
 * @param[in] user_call Used to know if we are recursing in the current function
 * call or not.
 *
 * @todo Because the function uses recustion, with low ms.difficulty (lots of
 * free cells, lots of recursive reveals) a stack overflow may occur, and the
 * whole system might crash :)
 */
static void reveal_tiles(int y, int x, bool user_call) {
    if (ms.grid[y * ms.w + x].c == BOMB_CH) {
        print_message("You lost. Press any key to restart.");
        ms.grid[y * ms.w + x].flags |= FLAG_CLEARED;
        ms.playing = PLAYING_FALSE;
        return;
    }

    if (adjacent_bombs(y, x) == 0) {
        /* If we have no bombs in surrounding tiles, make sure we mark the
         * current one as revealed so we don't have an endless loop when
         * recursing */
        ms.grid[y * ms.w + x].flags |= FLAG_CLEARED;

        const int start_y = (y > 0) ? y - 1 : y;
        const int start_x = (x > 0) ? x - 1 : x;
        const int end_y   = (y < ms.h - 1) ? y + 1 : y;
        const int end_x   = (x < ms.w - 1) ? x + 1 : x;

        /* No bombs in surrounding tiles, reveal them
         * ###
         * #X#
         * ### */
        for (int cur_y = start_y; cur_y <= end_y; cur_y++)
            for (int cur_x = start_x; cur_x <= end_x; cur_x++)
                /* If we are not revealing that one, reveal */
                if (!(ms.grid[cur_y * ms.w + cur_x].flags & FLAG_CLEARED))
                    reveal_tiles(cur_y, cur_x, false);
    } else {
        /* This part is for revealing adjacent tiles when clicking a revealed
         * one if all adjacent bombs have been flagged. We only care about this
         * feature if the user is the function caller.
         *
         * We have adjacent bombs, check if the tile we are tying to reveal was
         * already revealed */
        if (user_call && ms.grid[y * ms.w + x].flags & FLAG_CLEARED) {
#ifdef REVEAL_SURROUNDING
            /* If it was, check if all surrounding bombs have been flagged by
             * the user */
            if (surrounding_bombs_flagged(y, x)) {
                const int start_y = (y > 0) ? y - 1 : y;
                const int start_x = (x > 0) ? x - 1 : x;
                const int end_y   = (y < ms.h - 1) ? y + 1 : y;
                const int end_x   = (x < ms.w - 1) ? x + 1 : x;

                /* If they have been, we can automatically reveal all the other
                 * non-bomb adjacent tiles when clicking it. */
                for (int cur_y = start_y; cur_y <= end_y; cur_y++)
                    for (int cur_x = start_x; cur_x <= end_x; cur_x++)
                        if (ms.grid[cur_y * ms.w + cur_x].c != BOMB_CH)
                            reveal_tiles(cur_y, cur_x, false);
            }
#endif
        } else {
            /* If the current tile has bombs adjacent, but was not revealed,
             * reveal it */
            ms.grid[y * ms.w + x].flags |= FLAG_CLEARED;
        }
    }
}

/**
 * @brief Toggles the FLAG_FLAGGED bit of the tile at the specified position.
 * @param[in] fy, fx Position of the tile.
 */
static inline void toggle_flag(int fy, int fx) {
    if (ms.grid[fy * ms.w + fx].flags & FLAG_CLEARED) {
        print_message("Can't flag a revealed tile!");
        return;
    }

    ms.grid[fy * ms.w + fx].flags ^= FLAG_FLAGGED;
}

/**
 * @brief Check if the player won
 * @return True if all bombs have been flagged
 */
static inline bool check_win(void) {
    for (int y = 0; y < ms.h; y++)
        for (int x = 0; x < ms.w; x++)
            /* If there is an unflagged bomb, return false */
            if (ms.grid[y * ms.w + x].c == BOMB_CH &&
                !(ms.grid[y * ms.w + x].flags & FLAG_FLAGGED))
                return false;

    return true;
}

int main_minesweeper(int argc, char** argv) {
    /* Main minesweeper struct */
    ms = (Game){
        .w          = DEFAULT_W,
        .h          = DEFAULT_H,
        .grid       = NULL,
        .playing    = PLAYING_FALSE,
        .difficulty = DEFAULT_DIFFICULTY,
    };

    /* Parse arguments before curses */
    if (!parse_args(argc, argv))
        return 1;

    initscr(); /* Init curses */
    raw();     /* Scan input without pressing enter */
    noecho();  /* Don't print when typing */
#ifdef USE_ARROWS
    keypad(stdscr, true); /* Enable keypad (arrow keys) */
#endif

#ifdef USE_COLOR
    /* Global used to indicate redraw_grid that color is supported at runtime */
    use_color = has_colors();

    if (use_color) {
        start_color();

        init_pair(COL_NORM, COLOR_WHITE, COLOR_BLACK);
        init_pair(COL_1, COLOR_CYAN, COLOR_BLACK);
        init_pair(COL_2, COLOR_BLUE, COLOR_BLACK);
        init_pair(COL_3, COLOR_GREEN, COLOR_BLACK);
        init_pair(COL_4, COLOR_YELLOW, COLOR_BLACK);
        init_pair(COL_5, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(COL_6, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(COL_7, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(COL_8, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(COL_9, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(COL_FLAG, COLOR_RED, COLOR_BLACK);
        init_pair(COL_BOMB, COLOR_RED, COLOR_BLACK);
        init_pair(COL_UNK, COLOR_GRAY_B, COLOR_BLACK);
    }
#endif

    /* Init random seed */
    srand(time(NULL));

    /* Allocate and initialize grid */
    ms.grid = malloc(ms.w * ms.h * sizeof(Tile));
    init_grid();
    ms.playing = PLAYING_CLEAR;

    /* User cursor in the grid, not the screen. Start at the middle. */
    vec2_t cursor = {
        .y = (ms.h - 1) / 2,
        .x = (ms.w - 1) / 2,
    };

    redraw_grid(&cursor);

    /* Char the user is pressing */
    int c = 0;
    do {
        /* First, redraw the grid */
        redraw_grid(&cursor);

        /* Update the cursor (+margins) */
        move(cursor.y + 1, cursor.x + 1);

        /* Refresh screen */
        refresh();

        /* Wait for user input */
        c = tolower(getch());

        /* Clear the output line */
        clear_line(ms.h + 3);

        /* If it's the first iteration on a new game, clear grid. We will only
         * generate the bombs once we press space the first time */
        if (ms.playing == PLAYING_FALSE) {
            init_grid();
            ms.playing = PLAYING_CLEAR;
        }

        /* Parse input. 'q' quits and there is vim-like navigation */
        switch (c) {
            case 'k':
#ifdef USE_ARROWS
            case KEY_UP:
#endif
                if (cursor.y > 0)
                    cursor.y--;
                break;
            case 'j':
#ifdef USE_ARROWS
            case KEY_DOWN:
#endif
                if (cursor.y < ms.h - 1)
                    cursor.y++;
                break;
            case 'h':
#ifdef USE_ARROWS
            case KEY_LEFT:
#endif
                if (cursor.x > 0)
                    cursor.x--;
                break;
            case 'l':
#ifdef USE_ARROWS
            case KEY_RIGHT:
#endif
                if (cursor.x < ms.w - 1)
                    cursor.x++;
                break;
            case 'f':
                /* If we just started playing, but we don't have the bombs */
                if (ms.playing == PLAYING_CLEAR) {
                    print_message("Can't flag a tile before starting the "
                                  "game!");
                    break;
                }

                toggle_flag(cursor.y, cursor.x);

                if (check_win()) {
                    print_message("You won! Press any key to continue.");
                    ms.playing = PLAYING_FALSE;
                }

                break;
            case ' ':
                /* Initialize the bombs once we reveal for the first time */
                if (ms.playing == PLAYING_CLEAR) {
                    generate_grid(cursor, DIFFIC2BOMBPERCENT(ms.difficulty));
                    ms.playing = PLAYING_TRUE;
                } else if (ms.grid[cursor.y * ms.w + cursor.x].flags &
                           FLAG_FLAGGED) {
                    print_message("Can't reveal a flagged tile.");
                    break;
                }

                reveal_tiles(cursor.y, cursor.x, true);
                break;
            case 'r':
                /* Generate if it's the first time playing */
                if (ms.playing == PLAYING_CLEAR) {
                    generate_grid(cursor, DIFFIC2BOMBPERCENT(ms.difficulty));
                    ms.playing = PLAYING_TRUE;
                }

                print_message("Revealing all tiles and aborting game. "
                              "Press any key to continue.");

                for (int y = 0; y < ms.h; y++)
                    for (int x = 0; x < ms.w; x++)
                        ms.grid[y * ms.w + x].flags |= FLAG_CLEARED;

                ms.playing = PLAYING_FALSE;
                break;
            case KEY_CTRLC:
                c = 'q';
                break;
            case 'q':
            default:
                break;
        }
    } while (c != 'q');

    free(ms.grid);
    endwin();
    return 0;
}
