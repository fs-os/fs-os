
/*
 * For more information see:
 *   https://wiki.osdev.org/Bare_Bones#Implementing_the_Kernel
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <string.h>
#include <stdlib.h>
#include <kernel/tty.h>

#if defined(__linux__)
#error "You are not using a cross compiler." \
    "For more information see: https://github.com/fs-os/cross-compiler"
#endif

#if !defined(__i386__)
#error "You are not using a ix86-elf compiler." \
    "For more information see: https://github.com/fs-os/cross-compiler"
#endif

/* kernel_main: Called by boot.asm */
void kernel_main() {
    term_init();

    term_setcol(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    term_sprint("Hello, welcome to the Free and Simple Operating System!\n"
                "This project is still being developed. For more information, "
                "see:\n");
    term_setcol(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    term_sprint("https://github.com/fs-os/fs-os\n");
    term_setcol(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    /* Testing colors and scrolling */
    char buf[10] = { 0 };
    int fg       = 1;

    term_setcol(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    term_sprint("\nTesting colors, itoa, and terminal scrolling...\n");
    for (int i = 0; i <= 10; i++) {
        itoa(buf, i);

        term_setcol(fg, VGA_COLOR_BLACK);
        term_sprint(buf);
        term_putchar(' ');

        if (++fg > 15)
            fg = 1;
    }

    term_setcol(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    term_sprint("\n\nTesting more colors, itoan, and terminal scrolling...\n");
    for (int i = 100000; i <= 101000; i += 100) {
        itoan(buf, i, 5);

        term_setcol(fg, VGA_COLOR_BLACK);
        term_sprint(buf);
        term_putchar(' '); /* Replace with '\n' to test scrolling */

        if (++fg > 15)
            fg = 1;
    }
}

