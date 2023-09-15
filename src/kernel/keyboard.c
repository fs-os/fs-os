
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <kernel/keyboard.h>
#include <kernel/io.h>

/**
 * @brief Keyboard source
 * @details Filled layouts should be included here, other sources should declare
 * them extern.
 * @file
 */
#include <layouts/us.h>
#include <layouts/es.h>

/**
 * @brief Number of items of an array known at compile-time
 */
#define LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

/**
 * @brief IO ports used by the keyboard
 */
enum kb_io_ports {
    KB_PORT_DATA   = 0x60, /**< @brief Read/Write. Data port */
    KB_PORT_STATUS = 0x64, /**< @brief Read. Status port */
    KB_PORT_CMD    = 0x64, /**< @brief Write. Kb command port */
};

/**
 * @brief Flags for the KB_PORT_STATUS information.
 * @details See: https://wiki.osdev.org/%228042%22_PS/2_Controller
 */
enum kb_status_flags {
    KB_STATUS_BUFFER_OUT = 0x1, /**< @brief Output buffer status. 1 if full */
    KB_STATUS_BUFFER_IN  = 0x2, /**< @brief Input buffer status. 1 if full */
    KB_STATUS_SYSTEM     = 0x4, /**< @brief Should be cleared by firmware */
    KB_STATUS_CMD_DATA   = 0x8, /**< @brief If 0, data written to buffer is for
                                   the PS2 device, if 1, it's for the controller
                                   cmd. */
    KB_STATUS_UNK_0   = 0x10,   /**< @brief Chipset specific */
    KB_STATUS_UNK_1   = 0x20,   /**< @brief Chipset specific */
    KB_STATUS_TIMEOUT = 0x40,   /**< @brief If 1, timeout error */
    KB_STATUS_PARITY  = 0x80,   /**< @brief If 1, parity error */
};

/**
 * @brief Flags for each bit of the key_flags array
 */
enum kb_flags {
    KB_FLAG_PRESSED = 0x1, /**< @brief Will be 1 if the key is currently being
                             pressed */
};

/* -------------------------------------------------------------------------- */

/**
 * @brief Pointer to the current Layout struct being used.
 */
static const Layout* cur_layout = &us_layout;

/**
 * @brief Array of bytes containing information about each key state.
 * @details Each bit gives information about the key corresponding to its index.
 * For example: bit 0 of key_flags['c'] will be 1 if that key is pressed.
 */
static uint8_t key_flags[128] = { 0 };

/**
 * @brief Store if we should use caps
 */
static bool capslock_on = false, shift_held = false;

/**
 * @brief Store if we should print the characters to screen when receiving them.
 * @details Modified by kb_echo() and kb_noecho()
 */
static bool print_chars = true;

/**
 * @brief If true, kb_getchar() will wait for the user to input newline instead
 * of instantly returning each char.
 * @details Modified by kb_raw() and kb_noraw()
 */
static bool wait_for_eol = true;

/**
 * @brief True if a program is waiting for kb_getchar()
 */
static volatile bool getting_char = false;

/**
 * @name Buffers for the getchar functions.
 * @details Since kb_handler only reads 8 bit chars, we don't need bigger
 * arrays. We need to use signed chars because of EOF, though.
 * @{ */
static int8_t getchar_buf[KB_GETCHAR_BUFSZ]      = { EOF };
static uint16_t getchar_buf_pos                  = 0;
static int8_t getchar_line_buf[KB_GETCHAR_BUFSZ] = { EOF };
static uint16_t getchar_line_buf_pos             = 0;
/** @} */

/**
 * @brief Toggle variables like capslock_on or shift_held if needed
 * @param released Release bit from kb_handler()
 * @param key Key to be checked
 */
static inline void check_special(bool released, uint8_t key) {
    /* We can't use a case because they indexes are not constant at compile
     * time */
    if (key == cur_layout->special[KB_SPECIAL_IDX_LSHIFT] ||
        key == cur_layout->special[KB_SPECIAL_IDX_RSHIFT]) {
        /* Store that shift is being held */
        shift_held = !released;
    } else if (key == cur_layout->special[KB_SPECIAL_IDX_CAPSLOCK]) {
        /* Toggle capslock_on when we press the key */
        if (!released)
            capslock_on = !capslock_on;
    }
}

/**
 * @brief Returns the shift or normal keyboard layout depending if the shift is
 * pressed or not.
 * @return cur_layout->shift or cur_layout->def
 * @todo Only change letters with caps lock, only change special chars like '#'
 * with shift held.
 */
static inline unsigned char* get_layout(void) {
    return (capslock_on || shift_held) ? cur_layout->shift : cur_layout->def;
}

/* -------------------------------------------------------------------------- */

/* io_outb call is used to tell CPU that it's okay to resume interrupts. See:
 * https://wiki.osdev.org/Interrupts#From_the_OS.27s_perspective */
#define KB_HANDLER_RETURN()  \
    {                        \
        io_outb(0x20, 0x20); \
        return;              \
    }

void kb_handler(void) {
    uint8_t status = io_inb(KB_PORT_STATUS);
    if (!(status & KB_STATUS_BUFFER_OUT))
        KB_HANDLER_RETURN();

    uint8_t key = io_inb(KB_PORT_DATA);

    /* Highest bit is 1 if the key is released, store it and clear it from key
     */
    uint8_t released = (key >> 7) & 1;
    key &= 0x7f;

    /* Check if we should toggle global variables for caps, etc. */
    check_special(released, key);

    /* Check if we need to use an alternative layout when using shift, ctrl,
     * etc. */
    const unsigned char* final_layout = get_layout();
    const unsigned char final_key     = final_layout[key];

    /* Store the current key as pressed or released in the key_flags array */
    if (released) {
        key_flags[final_key] &= ~KB_FLAG_PRESSED;

        /* We only want to go to the next part if the key was pressed */
        KB_HANDLER_RETURN();
    } else {
        key_flags[final_key] |= KB_FLAG_PRESSED;
    }

    /* We only want to go to the next part if the key can be displayed */
    if (final_key == 0)
        KB_HANDLER_RETURN();

    /* We only want to go to the next part if a program called kb_getchar */
    if (!getting_char)
        KB_HANDLER_RETURN();

    /* If this variable is not set, kb_raw has been called. kb_getchar will need
     * to return each character inmediately, so we don't use the line buffer. */
    if (!wait_for_eol) {
        /* getchar_buf_pos will always be 0. See kb_getchar comment */
        getchar_buf[getchar_buf_pos++] = final_key;

        /* We don't use line buffer, we just print here and return */
        if (print_chars)
            putchar(final_key);

        KB_HANDLER_RETURN();
    }

    if (getchar_line_buf_pos >= KB_GETCHAR_BUFSZ)
        panic_line("getchar buffer out of bounds");

    /* Store the current char to the getchar line buffer */
    getchar_line_buf[getchar_line_buf_pos++] = final_key;

    /* If the key we just saved is '\n', the user is done with the input
     * line so we can move the chars to the final buffer */
    if (final_key == '\n') {
        for (int i = 0; i < getchar_line_buf_pos; i++) {
            getchar_buf[i]      = getchar_line_buf[i];
            getchar_line_buf[i] = EOF;
        }

        getchar_buf_pos      = 0; /* Not needed */
        getchar_line_buf_pos = 0;

        putchar(final_key);
    } else if (final_key == '\b') {
        /* Delete last char from line buffer if we detect '\b'. First, remove
         * the '\b' we just added. */
        getchar_line_buf[--getchar_line_buf_pos] = EOF;

        /* If we have something to delete, delete the last char */
        if (getchar_line_buf_pos > 0) {
            getchar_line_buf[--getchar_line_buf_pos] = EOF;

            /* Printing this to the fbc will delete last chararacter */
            putchar('\b');
        }
    } else if (print_chars) {
        /* We print the typed char here for:
         *  1. Only printing keyboard input when print_chars is true. Changed
         *     with kb_echo() and kb_noecho().
         *  2. Handle special cases (the conditional blocks above):
         *     - '\n': Just print.
         *     - '\b': Only print if we have something to delete. */
        putchar(final_key);
    }

    /* Tell CPU that it's okay to resume interrupts. See:
     * https://wiki.osdev.org/Interrupts#From_the_OS.27s_perspective */
    io_outb(0x20, 0x20);
}

bool kb_held(unsigned char c) {
    if (c >= 128)
        return 0;

    return key_flags[c] & KB_FLAG_PRESSED;
}

void kb_noecho(void) {
    print_chars = false;
}

void kb_echo(void) {
    print_chars = true;
}

bool kb_getecho(void) {
    return print_chars;
}

void kb_raw(void) {
    wait_for_eol = false;
}

void kb_noraw(void) {
    wait_for_eol = true;
}

bool kb_getraw(void) {
    /* Raw -> !wait_for_eol; NoRaw->wait_for_eol */
    return !wait_for_eol;
}

void kb_setlayout(const Layout* ptr) {
    cur_layout = ptr;
}

void kb_getchar_init(void) {
    for (size_t i = 0; i < LENGTH(getchar_buf); i++) {
        /* Both buffers should have the same length... */
        getchar_buf[i]      = EOF;
        getchar_line_buf[i] = EOF;
    }
}

int kb_getchar(void) {
    /* Tell the keyboard handler to store the key presses */
    getting_char = true;

    /* Wait until we read a valid char */
    volatile int8_t* tmp = &getchar_buf[getchar_buf_pos];
    while (*tmp == EOF)
        ;

    int c                        = getchar_buf[getchar_buf_pos];
    getchar_buf[getchar_buf_pos] = EOF;

    /* Try to increase the buffer position. If the next char is EOF, reset the
     * pos to 0. If wait_for_eol is false, getchar_buf_pos will always be 0
     * because the next char will always be EOF. */
    if (getchar_buf[++getchar_buf_pos] == EOF)
        getchar_buf_pos = 0;

    /* We don't want to read keys anymore (if we do, enable in next getchar
     * call) */
    getting_char = false;

    return c;
}
