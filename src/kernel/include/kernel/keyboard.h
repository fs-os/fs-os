
#ifndef KERNEL_KEYBOARD_H_
#define KERNEL_KEYBOARD_H_ 1

#include <stdint.h>
#include <stdbool.h>

/**
 * @def KB_GETCHAR_BUFSZ
 * @brief Max length of an input line
 */
#define KB_GETCHAR_BUFSZ 1000

/**
 * @enum kb_special_indexes
 * @brief Indexes of the Layout.special array.
 * @details For example:
 *   - us_layout.special will contain the key codes for special chars like shift
 *   - KB_SPECIAL_IDX_LSHIFT is 2 because the second item of the .special array
 *     will be the key code of the left shift key
 *   - us_layout.special[2] is 0x2A because 0x2A is the key code for left shift
 */
enum kb_special_indexes {
    KB_SPECIAL_IDX_ESC = 0,
    KB_SPECIAL_IDX_LCTRL,
    KB_SPECIAL_IDX_LSHIFT,
    KB_SPECIAL_IDX_RSHIFT,
    KB_SPECIAL_IDX_LALT,
    KB_SPECIAL_IDX_CAPSLOCK,
    KB_SPECIAL_IDX_F1,
    KB_SPECIAL_IDX_F2,
    KB_SPECIAL_IDX_F3,
    KB_SPECIAL_IDX_F4,
    KB_SPECIAL_IDX_F5,
    KB_SPECIAL_IDX_F6,
    KB_SPECIAL_IDX_F7,
    KB_SPECIAL_IDX_F8,
    KB_SPECIAL_IDX_F9,
    KB_SPECIAL_IDX_F10,
    KB_SPECIAL_IDX_NUMLOCK,
    KB_SPECIAL_IDX_SCROLL_LOCK,
    KB_SPECIAL_IDX_HOME,
    KB_SPECIAL_IDX_ARROW_UP,
    KB_SPECIAL_IDX_PAGE_UP,
    KB_SPECIAL_IDX_ARROW_LEFT,
    KB_SPECIAL_IDX_ARROW_RIGHT,
    KB_SPECIAL_IDX_END,
    KB_SPECIAL_IDX_ARROW_DOWN,
    KB_SPECIAL_IDX_PAGE_DOWN,
    KB_SPECIAL_IDX_INSERT,
    KB_SPECIAL_IDX_DELETE,
    KB_SPECIAL_IDX_F11,
    KB_SPECIAL_IDX_F12,
};

/**
 * @struct Layout
 * @brief Struct containing a pointer to the default and shift layouts for a
 * lang, plus an array of kb_special_indexes.
 */
typedef struct {
    unsigned char* def;   /**< @brief Chars to display for each key code */
    unsigned char* shift; /**< @brief Same as .def but when holding shift */
    uint16_t* special;    /**< @brief Array containing the indexes of special
                             chars for the current layout */
} Layout;

/* Layouts are included in src/keyboard.c */
extern const Layout us_layout;
extern const Layout es_layout;

/**
 * @brief Actual C handler for the keyboard exceptions received from irq_kb().
 * @details See src/kernel/idt.asm
 */
void kb_handler(void);

/**
 * @brief Check if \p c is being held.
 * @details key_flags is a static array defined in keyboard.c
 * @param[in] c Char to check.
 * @return True if the first bit of key_flags[c] is set.
 */
bool kb_held(unsigned char c);

/**
 * @brief Disable char printing on key press.
 * @details Sets the print_chars static var declared in keyboard.c
 */
void kb_noecho(void);

/**
 * @brief Enable char printing on key press.
 * @details Sets the print_chars static var declared in keyboard.c
 */
void kb_echo(void);

/**
 * @brief Get the static print_chars variable.
 * @details The print_chars static var is declared in keyboard.c
 * @return True if we are displaying chars on input.
 */
bool kb_getecho(void);

/**
 * @brief Don't wait for newline when getting chars.
 * @details Sets the wait_for_eol static var declared in keyboard.c
 */
void kb_raw(void);

/**
 * @brief Wait for newline when getting chars.
 * @details Sets the wait_for_eol static var declared in keyboard.c
 */
void kb_noraw(void);

/**
 * @brief Get the static wait_for_eol variable.
 * @details The wait_for_eol static var is declared in keyboard.c
 * @return True if we are waiting for end of line before returning chars.
 */
bool kb_getraw(void);

/**
 * @brief Set the current active keyboard layout to the specified Layout ptr
 * @param[in] ptr Description
 */
void kb_setlayout(const Layout* ptr);

/**
 * @brief Initialize the static getchar arrays to EOF.
 */
void kb_getchar_init(void);

/**
 * @brief Get input chars from the keyboard once the user wrote a line
 * @details First it tells the kb_handler function that we are waiting for
 * input, then read from the getchar_buf until there is a char. If wait_for_eol
 * is false, getchar_buf will be filled instantly, otherwise it will wait for
 * the user to type a full line.
 *
 * See the fs-os wiki for more info.
 * @return Next user input char from the keyboard.
 */
int kb_getchar(void);

#endif /* KERNEL_KEYBOARD_H_ */
