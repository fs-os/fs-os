
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

#define TEST_TITLE(s)                                       \
    {                                                       \
        term_setcol(VGA_COLOR_WHITE, VGA_COLOR_BLACK);      \
        term_sprint(s);                                     \
        term_setcol(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK); \
    }

/* test_libk: called by kernel_main to test libk functions */
static inline void test_libk() {
    char buf[255] = { 0 };

    TEST_TITLE("\nTesting colors, itoa, itoan, and terminal scrolling...\n");
    for (int fg = 0; fg <= 15; fg++) {
        itoa(buf, fg);

        if (fg == VGA_COLOR_BLACK)
            term_setcol(fg, VGA_COLOR_LIGHT_GREY);
        else
            term_setcol(fg, VGA_COLOR_BLACK);

        term_sprint(buf);

        term_setcol(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        term_putchar(' ');
    }

    TEST_TITLE("\n\nTesting string.h functions...\n");

    term_sprint("strlen(\"abcd\") -> ");
    itoa(buf, strlen("abcd"));
    term_sprint(buf);
    term_putchar('\n');

    term_sprint("memcmp(\"abcd\", \"abca\", 4) -> ");
    itoa(buf, memcmp("abcd", "abc1", 4));
    term_sprint(buf);
    term_putchar('\n');

    term_sprint("memcmp(\"abcd\", \"abce\", 4) -> ");
    itoa(buf, memcmp("abcd", "abce", 4));
    term_sprint(buf);
    term_putchar('\n');

    term_sprint("memcmp(\"123456\", \"123456\", 6) -> ");
    itoa(buf, memcmp("123456", "123456", 6));
    term_sprint(buf);
    term_putchar('\n');

    term_sprint("memset(buf, 'h', 5) -> ");
    memset(buf, 'h', 5);
    buf[5] = '\0';
    term_sprint(buf);
    term_putchar('\n');

    term_sprint("memcpy(&buf[5], &buf[0], 5) -> ");
    memcpy(&buf[5], &buf[0], 5);
    buf[10] = '\0';
    term_sprint(buf);
    term_putchar('\n');
}

/* kernel_main: Called by boot.asm */
void kernel_main() {
    term_init();

    term_setcol(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
    term_sprint("Hello, welcome to the Free and Simple Operating System!\n"
                "This project is still being developed. For more information, "
                "see:\n");
    term_setcol(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
    term_sprint("https://github.com/fs-os/fs-os\n");

    test_libk();
}

