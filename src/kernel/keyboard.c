
#include <stdint.h>
#include <stdio.h>
#include <kernel/keyboard.h>
#include <kernel/io.h>

#include <layouts/en.h>

enum kb_io_ports {
    KB_PORT_DATA   = 0x60, /* Read/Write. Data port */
    KB_PORT_STATUS = 0x64, /* Read. Status port */
    KB_PORT_CMD    = 0x64, /* Write. Command port */
};

/* See https://wiki.osdev.org/%228042%22_PS/2_Controller */
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

static char* cur_layout = en_layout.def;

/* check_layout: change the cur_layout to lang_layout.shift when we detect the shift
 * is pressed, or to lang_layout.def when shift is released; toggle when caps lock is
 * pressed, etc. */
static inline void check_layout(uint8_t released, uint8_t key) {
    /* TODO */
}

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

        /* Check if we need to use an alternative layout when using shift, etc. */
        /* check_layout(released, key); */

        /* Check if we are pressing a key (not releasing) and if the current layout
         * has a char to display, and print it */
        /* TODO: When printing '\b', check if we put the char so we can't delete
         * strings printed by other programs */
        if (!released && cur_layout[key] != 0)
            putchar(cur_layout[key]);

        status = io_inb(KB_PORT_STATUS);
    }

    /* Tell CPU that it's okay to resume interrupts. See:
     * https://wiki.osdev.org/Interrupts#From_the_OS.27s_perspective */
    io_outb(0x20, 0x20);
}

