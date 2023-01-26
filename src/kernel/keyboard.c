
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <kernel/keyboard.h>
#include <kernel/io.h>

#include <layouts/en.h>

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
static const Layout* cur_layout = &en_layout;

/* Array of bytes contaning information about each key state. Each bit gives
 * information about the key corresponding to its index. For example: bit 0 of
 * key_flags['c'] will be 1 if that key is pressed. */
static uint8_t key_flags[128] = { 0 };

/* Store if we should use caps */
static bool capslock_on = 0, shift_held = 0;

/* Store if we should print the characters to screen when recieving them */
static bool print_chars = 1;

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
 * lang_layout.def when shift is not being usd */
static inline char* get_layout(void) {
    /* TODO: We should only change letters with caps lock, special chars like '#'
     * should only be changed with shift. */
    return (capslock_on || shift_held) ? cur_layout->shift : cur_layout->def;
}

/* ------------------------------------------------------------------------------- */

/* kb_handler: actual C handler for the keyboard exceptions recieved from "irq_kb".
 * See src/kernel/idt_asm.asm */
void kb_handler(void) {
    /* The ps2 controller wiki page says (at the bottom, interrupts) that you don't
     * really need to read the bit 0 of the status byte when reading port 0x60, but
     * I have seen it in other projects so I am going to do it. */
    uint8_t status = io_inb(KB_PORT_STATUS);

    while (status & KB_STATUS_BUFFER_OUT) {
        uint8_t key = io_inb(KB_PORT_DATA);

        /* Highest bit is 1 if the key is released, store it and clear it from key */
        uint8_t released = (key >> 7) & 1;
        key &= 0x7f;

        /* Store the current key as pressed or released in the key_flags array */
        if (released)
            key_flags[key] &= ~KB_FLAG_PRESSED;
        else
            key_flags[key] |= KB_FLAG_PRESSED;

        /* Check if we should toggle global variables for caps, etc. */
        check_special(released, key);

        /* Check if we need to use an alternative layout when using shift, etc. */
        const char* final_layout = get_layout();

        /* Check if we are pressing a key (not releasing) and if the current layout
         * has a char to display, and print it */
        /* TODO: When printing '\b', check if we put the char so we can't delete
         * strings printed by other programs */
        if (print_chars && !released && final_layout[key] != 0)
            putchar(final_layout[key]);

        status = io_inb(KB_PORT_STATUS);
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

/* kb_setlayout: set the current active layout to the specified Layout pointer */
void kb_setlayout(const Layout* ptr) {
    cur_layout = ptr;
}

