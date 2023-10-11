
#ifndef APPS_MINESWEEPER_DEFINES_H_
#define APPS_MINESWEEPER_DEFINES_H_ 1

/**
 * @def REVEAL_SURROUNDING
 * @brief Compile with "reveal surrounding" feature
 * @details If defined, adds the "reveal surrounding" feature. With this
 * feature, adjacent tiles will get revealed if the user is trying to reveal:
 *   - An already revealed tile
 *   - With adjacent bombs
 *   - With all bombs flagged
 * Comment this line if you don't want this feature.
 */
#define REVEAL_SURROUNDING

/**
 * @def USE_ARROWS
 * @brief Enable arrow key support.
 * @details If you compile the program with `USE_ARROWS`, you will be able to
 * navigate with the arrows.
 */
/* #define USE_ARROWS */

/**
 * @def USE_COLOR
 * @brief Compile with color enabled.
 * @details If you compile the program with `USE_COLOR` and your terminal
 * supports it, it will render the tiles with color.
 */
#define USE_COLOR

/**
 * @enum color_ids
 * @brief Color ids used for the curses color pairs.
 */
enum color_ids {
    COL_NORM = 0,  /**< @brief White */
    COL_1    = 1,  /**< @brief 1 bombs adjacent */
    COL_2    = 2,  /**< @brief 2 bombs adjacent */
    COL_3    = 3,  /**< @brief 3 bombs adjacent */
    COL_4    = 4,  /**< @brief 4 bombs adjacent */
    COL_5    = 5,  /**< @brief 5 bombs adjacent */
    COL_6    = 6,  /**< @brief 6 bombs adjacent */
    COL_7    = 7,  /**< @brief 7 bombs adjacent */
    COL_8    = 8,  /**< @brief 8 bombs adjacent */
    COL_9    = 9,  /**< @brief 9 bombs adjacent */
    COL_FLAG = 10, /**< @brief Red  */
    COL_BOMB = 11, /**< @brief Red  */
    COL_UNK  = 12, /**< @brief Gray */
};

/**
 * @name Color macros
 * Will only work if color is enabled and supported. Bold is not yet supported
 * in fs-os.
 * @{ */
#define SET_COL(col)   \
    if (use_color) {   \
        use_pair(col); \
    }

#define RESET_COL()   \
    if (use_color) {  \
        reset_pair(); \
    }

#define INVERT_COL(col)   \
    if (use_color) {      \
        invert_pair(col); \
    }

/* Bold is not supported in the fs-os terminal yet */
#define BOLD_ON()
#define BOLD_OFF()

#define DEFAULT_W 50 /**< @brief Default width */
#define DEFAULT_H 20 /**< @brief Default height */
#define MIN_W     10 /**< @brief Minimum width */
#define MIN_H     10 /**< @brief Minimum height */

/**
 * @def BOMB_MARGIN
 * @brief Bombs won't spawn in a 3x3 grid from the starting reveal
 */
#define BOMB_MARGIN 3

#define DEFAULT_DIFFICULTY 30 /**< @brief 1-100% */
#define MIN_BOMBS          5  /**< @brief Minimum ammount of bombs */
#define MAX_BOMBS          60 /**< @brief Maximum ammount of bombs */

/**
 * @enum tile_flags
 * @brief For tile_t.flags
 */
enum tile_flags {
    FLAG_NONE    = 0x0, /**< @brief Tile is hidden and not flagged */
    FLAG_CLEARED = 0x1, /**< @brief Tile is revealed */
    FLAG_FLAGGED = 0x2, /**< @brief Tile is flagged */
};

/**
 * @enum playing_flags
 * @brief For ms_t.playing
 */
enum playing_flags {
    PLAYING_FALSE = 0x0, /**< @brief Just started a game. Grid empty. */
    PLAYING_TRUE  = 0x1, /**< @brief Playing a game. Grid filled. */
    PLAYING_CLEAR = 0x2, /**< @brief Just initialized the empty grid. Fill with
                            bombs */
};

/**
 * @enum tile_chars
 * @brief Characters for the tiles
 */
enum tile_chars {
    UNKN_CH = '.', /* Not revealed tile */
    BACK_CH = ' ', /* Empty tile with no bombs adjacent */
    BOMB_CH = '@', /* Revealed tile with bomb */
    FLAG_CH = 'F', /* Flagged hidden tile */
};

/**
 * @def KEY_CTRLC
 * @brief Needed for getch()
 */
#define KEY_CTRLC 3

#endif /* APPS_MINESWEEPER_DEFINES_H_ */
