
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
#define REVEAL_TILE(P)        ms.grid[P.y * ms.w + P.x].flags |= FLAG_CLEARED

/**
 * @struct Tile
 * @brief Tile of the minesweeper
 * @description The char indicates its contents and the flags special
 * information (for example if the user flagged the tile, revealed it...)
 */
typedef struct {
    char c;        /* Char in that tile, actual item */
    uint8_t flags; /* Tile status (revealed, flagged, etc) */
} Tile;

/**
 * @struct Game
 * @brief Minesweeper struct containing the game information
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
    int32_t x, y;
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

/**
 * @var queue
 * @brief Global vec2_t queue
 * @details Used by reveal_tiles() to avoid recursion. Allocated and freed from
 * main()
 */
static vec2_t* queue = NULL;

/**
 * @var queue_pos
 * @brief Top of the queue
 * @details Index in the queue that will be used for the next pushed item, not
 * the index of the last item.
 */
static int queue_pos = 0;

/*----------------------------------------------------------------------------*/

/**
 * @brief Parses a resolution string with format `WIDTHxHEIGHT` using atoi
 * @details Writes an extra '\0' to src
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
 * @brief Parses the program arguments changing the properties of ms
 * @param[in] argc Number of arguments from main
 * @param[in] argv String vector with the artuments
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
            return false;
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
 * @brief Draws the grid border for the minesweeper
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
            ms.grid[y * ms.w + x].c     = CH_BACK;
            ms.grid[y * ms.w + x].flags = FLAG_NONE;
        }
    }
}

/**
 * @brief Returns the number of bombs adjacent to a specified tile
 * @details Adjacent meaning in a 3x3 grid with the speicified tile at its
 * center.
 * @param[in] p Position of the tile to check
 * @return Number of bombs adjacent
 */
static int adjacent_bombs(vec2_t p) {
    int ret = 0;

    const vec2_t start = {
        .y = (p.y > 0) ? p.y - 1 : p.y,
        .x = (p.x > 0) ? p.x - 1 : p.x,
    };

    const vec2_t end = {
        .y = (p.y < ms.h - 1) ? p.y + 1 : p.y,
        .x = (p.x < ms.w - 1) ? p.x + 1 : p.x,
    };

    /* ###
     * #X#
     * ### */
    vec2_t cur;
    for (cur.y = start.y; cur.y <= end.y; cur.y++)
        for (cur.x = start.x; cur.x <= end.x; cur.x++)
            if (ms.grid[cur.y * ms.w + cur.x].c == CH_BOMB)
                ret++;

    return ret;
}

/**
 * @brief Prints the specified message 2 lines bellow `ms`'s grid
 * @param[in] str String to be printed
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
 * @brief Clears a line in the screen
 * @details Doesn't change the cursor position
 * @param[in] y Line number starting from 0 to clear
 */
static inline void clear_line(int y) {
    int oy, ox;
    getyx(stdscr, oy, ox);

    move(y, 0);
    clrtoeol();

    move(oy, ox);
}

/**
 * @brief Redraws the grid based on the ms.grid array
 * @details Color macros will only do something if color is enabled and
 * supported.
 * @param[in] cursor Current cursor location for inverting the color
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
                const int bombs = adjacent_bombs((vec2_t){ x, y });
                if (ms.grid[y * ms.w + x].c == CH_BOMB) {
                    /* Bomb (we lost) */
                    final_col = COL_BOMB;
                    final_ch  = CH_BOMB;
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
                final_ch  = CH_FLAG;
            } else {
                final_col = COL_UNK;
                final_ch  = CH_UNKN;
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
 * @brief Fill the grid with bombs at random locations
 * @details Will leave a margin area around the first user selection (so it
 * never reveals a bomb on the first input).
 * @param[in] start Position of the first tile that the user tried to reveal
 * @param[in] bomb_percent The percentage of bombs to fill
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

        ms.grid[bomb_y * ms.w + bomb_x].c = CH_BOMB;
    }
}

/**
 * @brief Returns true if all adjacent bombs from a cell have been flagged
 * @details Assumes adjacent_bombs() has been called and it didn't return 0
 * @param[in] y, x Position to check
 * @return True if all adjacent bombs have been flagged by the user
 */
static inline bool surrounding_bombs_flagged(vec2_t p) {
    /* NOTE: Assumes adjacent_bombs() has been called and it didn't return 0 */

    const vec2_t start = {
        .y = (p.y > 0) ? p.y - 1 : p.y,
        .x = (p.x > 0) ? p.x - 1 : p.x,
    };

    const vec2_t end = {
        .y = (p.y < ms.h - 1) ? p.y + 1 : p.y,
        .x = (p.x < ms.w - 1) ? p.x + 1 : p.x,
    };

    vec2_t cur;
    for (cur.y = start.y; cur.y <= end.y; cur.y++)
        for (cur.x = start.x; cur.x <= end.x; cur.x++)
            /* We found an adjacent bomb and it was not flagged */
            if (ms.grid[cur.y * ms.w + cur.x].c == CH_BOMB &&
                !(ms.grid[cur.y * ms.w + cur.x].flags & FLAG_FLAGGED))
                return false;

    return true;
}

/**
 * @brief Check if the specified tile doesn't have the CLEARED flag set
 * @param[in] p Position to check
 * @return True if a valid position in the grid and if the CLEARED flag is not
 * set
 */
static inline bool is_hidden(vec2_t p) {
    return p.x >= 0 && p.x < ms.w && p.y >= 0 && p.y < ms.h &&
           !(ms.grid[p.y * ms.w + p.x].flags & FLAG_CLEARED);
}

/**
 * @brief Check if the specified tile has any adjacent bombs
 * @param[in] p Position to check
 * @return True if a valid position in the grid and if it has no adjacent bombs
 */
static inline bool is_empty(vec2_t p) {
    return p.x >= 0 && p.x < ms.w && p.y >= 0 && p.y < ms.h &&
           adjacent_bombs(p) == 0;
}

/**
 * @brief Pop value from the front of a 2d vector queue
 * @details Used by reveal_tiles()
 * @return Value at the front (index 0) of the queue
 */
static inline vec2_t queue_pop_front() {
    vec2_t ret = queue[0];

    /* Shift. Note that `top` is not the last pushed value, but the next one */
    queue_pos--;
    for (int i = 0; i < queue_pos; i++)
        queue[i] = queue[i + 1];

    return ret;
}

/**
 * @brief Push value to the back of a 2d vector queue
 * @details Used by reveal_tiles()
 * @param[in] x Value to push
 */
static inline void queue_push(vec2_t x) {
    queue[queue_pos++] = x;
}

/**
 * @brief Reveals the needed tiles using recursion, starting at y and x
 * @param[in] y, x Position to be revealed
 * @param[in] user_call Used to know if we are recursing in the current function
 * call or not
 *
 * @todo Don't malloc and free the queue in each call, alloc once as global
 */
void reveal_tiles(vec2_t p, bool user_call) {
    if (user_call && ms.grid[p.y * ms.w + p.x].c == CH_BOMB) {
        print_message("You lost. Press any key to restart.");
        ms.grid[p.y * ms.w + p.x].flags |= FLAG_CLEARED;
        ms.playing = PLAYING_FALSE;
        return;
    }

    REVEAL_TILE(p);

    /* Current tile has no number in it */
    if (adjacent_bombs(p) == 0) {
        /* Queue is allocated once globally */
        queue_pos = 0;

        /* Push parameter and reveal it */
        queue_push(p);

        /* Queue is not empty */
        while (queue_pos > 0) {
            const vec2_t cur = queue_pop_front();

            /* Iterate adjacent, avoiding the middle tile (cur).
             *  ...
             *  .p.
             *  ...  */
            vec2_t iter;
            for (iter.y = cur.y - 1; iter.y <= cur.y + 1; iter.y++) {
                for (iter.x = cur.x - 1; iter.x <= cur.x + 1; iter.x++) {
                    /* Skip middle tile */
                    if (iter.x == cur.x && iter.y == cur.y)
                        continue;

                    /* If hidden, reveal */
                    if (is_hidden(iter)) {
                        REVEAL_TILE(iter);

                        /* If we encounter another empty tile, push queue */
                        if (is_empty(iter))
                            queue_push(iter);
                    }
                }
            }
        }
    } else if (user_call && (ms.grid[p.y * ms.w + p.x].flags & FLAG_CLEARED) &&
               surrounding_bombs_flagged(p)) {
#ifdef REVEAL_SURROUNDING
        /*
         * If the user is trying to reveal an already cleared flag, and all
         * adjacent bombs are flagged, auto-reveal surrounding.
         *
         * We only care about this feature if the user is the function caller
         * (we are not recursing). Note that we only use recursion here (with
         * less than 8 calls) to avoid a stack overflow.
         */

        const vec2_t start = {
            .y = (p.y > 0) ? p.y - 1 : p.y,
            .x = (p.x > 0) ? p.x - 1 : p.x,
        };

        const vec2_t end = {
            .y = (p.y < ms.h - 1) ? p.y + 1 : p.y,
            .x = (p.x < ms.w - 1) ? p.x + 1 : p.x,
        };

        /* Iterate and reveal X, avoiding bombs and the middle tile:
         *  ..@
         *  .p.
         *  .@.  */
        vec2_t cur;
        for (cur.y = start.y; cur.y <= end.y; cur.y++)
            for (cur.x = start.x; cur.x <= end.x; cur.x++)
                if (ms.grid[cur.y * ms.w + cur.x].c != CH_BOMB &&
                    (cur.x != p.x || cur.y != p.y))
                    reveal_tiles(cur, false);
#endif
    }
}

/**
 * @brief Toggles the FLAG_FLAGGED bit of the tile at the specified position
 * @param[in] p Position of the tile
 */
static inline void toggle_flag(vec2_t p) {
    if (ms.grid[p.y * ms.w + p.x].flags & FLAG_CLEARED) {
        print_message("Can't flag a revealed tile!");
        return;
    }

    ms.grid[p.y * ms.w + p.x].flags ^= FLAG_FLAGGED;
}

/**
 * @brief Check if the player won
 * @return True if all bombs have been flagged
 */
static inline bool check_win(void) {
    for (int y = 0; y < ms.h; y++)
        for (int x = 0; x < ms.w; x++)
            /* If there is an unflagged bomb, return false */
            if (ms.grid[y * ms.w + x].c == CH_BOMB &&
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

    /* Allocate global queue used by reveal_tiles() */
    queue = malloc(ms.w * ms.h * sizeof(vec2_t));

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

                toggle_flag(cursor);

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

                reveal_tiles(cursor, true);
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

    free(queue);
    free(ms.grid);
    endwin();
    return 0;
}
