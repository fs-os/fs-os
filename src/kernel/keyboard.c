
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <kernel/keyboard.h>
#include <kernel/io.h>

#include <layouts/us.h>

#define LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

/* IO ports used by the keyboard */
enum kb_io_ports {
    KB_PORT_DATA   = 0x60, /* Read/Write. Data port */
    KB_PORT_STATUS = 0x64, /* Read. Status port */
    KB_PORT_CMD    = 0x64, /* Write. Command port */
};

/* Flags for the KB_PORT_STATUS information. See:
 * https://wiki.osdev.org/%228042%22_PS/2_Controller */
enum kb_status_flags {
    KB_STATUS_BUFFER_OUT = 0x1, /* 00000001. Output buffer status. 1 if full */
    KB_STATUS_BUFFER_IN  = 0x2, /* 00000010. Input buffer status. 1 if full */
    KB_STATUS_SYSTEM     = 0x4, /* 00000100. Should be cleared by firmware */
    KB_STATUS_CMD_DATA   = 0x8, /* 00001000. If 0, data written to buffer is for the
                                   PS2 device, if 1, it's for the controller cmd */
    KB_STATUS_UNK_0   = 0x10,   /* 00010000. Chipset specific */
    KB_STATUS_UNK_1   = 0x20,   /* 00100000. Chipset specific */
    KB_STATUS_TIMEOUT = 0x40,   /* 01000000. If 1, timeout error */
    KB_STATUS_PARITY  = 0x80,   /* 10000000. If 1, parity error */
};

/* Flags for each bit of the key_flags array */
enum kb_flags {
    KB_FLAG_PRESSED = 0x1, /* 00000001. Will be 1 if the key is currently pressed */
};

/* ------------------------------------------------------------------------------- */

/* Pointer to the current Layout struct being used */
static const Layout* cur_layout = &us_layout;

/* Array of bytes containing information about each key state. Each bit gives
 * information about the key corresponding to its index. For example: bit 0 of
 * key_flags['c'] will be 1 if that key is pressed. */
static uint8_t key_flags[128] = { 0 };

/* Store if we should use caps */
static bool capslock_on = false, shift_held = false;

/* Store if we should print the characters to screen when receiving them */
static bool print_chars = true;

/* True if a program is waiting for kb_getchar */
static volatile bool getting_char = false;

/* Buffers for the getchar functions. Since kb_handler only reads 8 bit chars, we
 * don't need bigger arrays. We need to use signed chars because of EOF, though */
static int8_t getchar_buf[KB_GETCHAR_BUFSZ]      = { EOF };
static uint16_t getchar_buf_pos                  = 0;
static int8_t getchar_line_buf[KB_GETCHAR_BUFSZ] = { EOF };
static uint16_t getchar_line_buf_pos             = 0;

/* check_special: toggle variables like capslock_on or shift_held if needed */
static inline void check_special(uint8_t released, uint8_t key) {
    /* We can't use a case because they indexes are not constant at compile time */
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

/* check_layout: return lang_layout.shift if the shift is pressed, or to
 * lang_layout.def when shift is not being used */
static inline unsigned char* get_layout(void) {
    /* TODO: We should only change letters with caps lock, special chars like '#'
     * should only be changed with shift. */
    return (capslock_on || shift_held) ? cur_layout->shift : cur_layout->def;
}

/* ------------------------------------------------------------------------------- */

/* kb_handler: actual C handler for the keyboard exceptions received from "irq_kb".
 * See src/kernel/idt_asm.asm */
void kb_handler(void) {
    /* The ps2 controller wiki page says (at the bottom, interrupts) that you don't
     * really need to read the bit 0 of the status byte when reading port 0x60, but
     * I have seen it in other projects so I am going to do it. */
    for (uint8_t status = io_inb(KB_PORT_STATUS); status & KB_STATUS_BUFFER_OUT;
         status         = io_inb(KB_PORT_STATUS)) {
        uint8_t key = io_inb(KB_PORT_DATA);

        /* Highest bit is 1 if the key is released, store it and clear it from key */
        uint8_t released = (key >> 7) & 1;
        key &= 0x7f;

        /* Check if we should toggle global variables for caps, etc. */
        check_special(released, key);

        /* Check if we need to use an alternative layout when using shift, etc. */
        const unsigned char* final_layout = get_layout();
        const unsigned char final_key     = final_layout[key];

        /* Store the current key as pressed or released in the key_flags array */
        if (released) {
            key_flags[final_key] &= ~KB_FLAG_PRESSED;

            /* We only want to go to the next part if the key was pressed */
            continue;
        } else {
            key_flags[final_key] |= KB_FLAG_PRESSED;
        }

        /* We only want to go to the next part if the key can be displayed */
        if (final_key == 0)
            continue;

        /* If a program called kb_getchar */
        if (getting_char) {
            /* Check if we are pressing a key (not releasing) and if the current
             * layout has a char to display, and print it */
            if (getchar_line_buf_pos >= KB_GETCHAR_BUFSZ)
                panic_line("getchar buffer out of bounds");

            /* Store the current char to the getchar line buffer (if the char can be
             * displayed with the current font) */
            getchar_line_buf[getchar_line_buf_pos++] = final_key;

            /* Check if the key we just saved is '\n'. If it is, the user is done
             * with the input line so we can move the chars to the final buffer */
            if (final_key == '\n') {
                for (int i = 0; i < getchar_line_buf_pos; i++) {
                    getchar_buf[i]      = getchar_line_buf[i];
                    getchar_line_buf[i] = EOF;
                }

                getchar_buf_pos      = 0; /* Not needed */
                getchar_line_buf_pos = 0;

                putchar(final_key);
            } else if (final_key == '\b') {
                /* Delete last char from line buffer if we detect '\b' */

                /* Remove the '\b' we just added */
                getchar_line_buf[--getchar_line_buf_pos] = EOF;

                /* Delete the last char and print '\b', only if we have something to
                 * delete */
                if (getchar_line_buf_pos > 0) {
                    getchar_line_buf[--getchar_line_buf_pos] = EOF;
                    putchar(final_key);
                }
            } else if (print_chars) {
                /* We print the typed char here for:
                 *   - Only printing keyboard input when a program asks for it
                 *   - Handle special chars like '\n' (not needed) or '\b' (only
                 *     print if we have something to delete) */
                putchar(final_key);
            }
        }
    }

    /* Tell CPU that it's okay to resume interrupts. See:
     * https://wiki.osdev.org/Interrupts#From_the_OS.27s_perspective */
    io_outb(0x20, 0x20);
}

/* kb_held: check if "c" is being held. Returns 1 if the first bit of key_flags[c] is
 * set. */
bool kb_held(unsigned char c) {
    if (c >= 128)
        return 0;

    return key_flags[c] & KB_FLAG_PRESSED;
}

/* kb_noecho: disable char printing on key press */
void kb_noecho(void) {
    print_chars = false;
}

/* kb_echo: enable char printing on key press */
void kb_echo(void) {
    print_chars = true;
}

/* kb_getecho: get the static print_chars variable */
bool kb_getecho(void) {
    return print_chars;
}

/* kb_setlayout: set the current active layout to the specified Layout pointer */
void kb_setlayout(const Layout* ptr) {
    cur_layout = ptr;
}

/* kb_getchar_init: initialize the static getchar arrays to EOF */
void kb_getchar_init(void) {
    for (size_t i = 0; i < LENGTH(getchar_buf); i++) {
        /* Both buffers should have the same length... */
        getchar_buf[i]      = EOF;
        getchar_line_buf[i] = EOF;
    }
}

/* kb_getchar: get input chars from "getchar_buf" once the user wrote a line */
int kb_getchar(void) {
    /* Tell the keyboard handler to store the key presses */
    getting_char = true;

    /* Wait until we read a valid char */
    volatile int8_t* tmp = &getchar_buf[getchar_buf_pos];
    while (*tmp == EOF)
        ;

    int c                        = getchar_buf[getchar_buf_pos];
    getchar_buf[getchar_buf_pos] = EOF;

    /* Try to increase the buffer position. If the next char is EOF, reset the pos to
     * 0 */
    if (getchar_buf[++getchar_buf_pos] == EOF)
        getchar_buf_pos = 0;

    /* We don't want to read keys anymore (if we do, enable in next getchar call) */
    getting_char = false;

    return c;
}

