
#ifndef APPS_5x5_DEFINES_H_
#define APPS_5x5_DEFINES_H_ 1

/**
 * @def CHEAT
 * @brief Compile with cheater features.
 * @details If the program is compiled with this macro defined, it will add the
 * "Generate cheat grid" feature, which generates a very easy game for testing.
 */
#define CHEAT

/**
 * @def USE_COLOR
 * @brief Compile with color enabled.
 * @details If you compile the program with `USE_COLOR` and your terminal
 * supports it, it will render the tiles with color.
 */
#define USE_COLOR

#define DEFAULT_H   5 /**< @brief Default height */
#define DEFAULT_W   5 /**< @brief Default width */
#define DEFAULT_YSC 3 /**< @brief Default vertical scale */
#define DEFAULT_XSC 4 /**< @brief Default horizontal scale */
#define MIN_W       5 /**< @brief Minimum width */
#define MIN_H       5 /**< @brief Minimum height */

/**
 * @def KEY_CTRLC
 * @brief Needed for getch()
 */
#define KEY_CTRLC 3

/**
 * @enum game_chars
 * @brief Characters for the tiles
 */
enum game_chars {
    ON_CH  = '#', /**< @brief On tile */
    OFF_CH = '.', /**< @brief Off tile */
};

/**
 * @enum color_pairs
 * @brief Color pair numbers for curses
 */
enum color_pairs {
    COL_GRAY  = 1, /**< @brief For borders */
    COL_WHITE = 2, /**< @brief For inverting cursor */
};

/**
 * @name Color macros
 * Will only work if color is enabled and supported. Bold is not yet supported
 * in fs-os.
 * @{ */
#define SET_COL(col)       \
    {                      \
        if (use_color) {   \
            use_pair(col); \
        }                  \
    }

#define RESET_COL()       \
    {                     \
        if (use_color) {  \
            reset_pair(); \
        }                 \
    }

#define INVERT_COL(col)       \
    {                         \
        if (use_color) {      \
            invert_pair(col); \
        }                     \
    }
/** @} */

/**
 * @struct point_t
 * @brief Point in the **context grid**.
 * @details Not a point on the real terminal, but on the grid. This is useful
 * for moving to the real terminal position after rendering the scalled grid.
 */
typedef struct {
    uint16_t y; /**< @brief Y coordinate */
    uint16_t x; /**< @brief X coordinate */
} point_t;

/**
 * @struct ctx_t
 * @brief Context used for the game.
 */
typedef struct {
    uint16_t h;     /**< @brief Grid height */
    uint16_t w;     /**< @brief Grid width */
    uint16_t ysc;   /**< @brief Vertical grid scale for rendering */
    uint16_t xsc;   /**< @brief Horizontal scale for rendering */
    point_t cursor; /**< @brief User position inside the grid */

    /**
     * @brief Pointer to the game grid.
     * @details It will save 1 char per cell, scale will be used for rendering
     * only.
     * */
    uint8_t* grid;
} ctx_t;

#endif /* APPS_5x5_DEFINES_H_ */
