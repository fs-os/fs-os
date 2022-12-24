
/*
 * For more information see:
 *   https://wiki.osdev.org/Bare_Bones#Implementing_the_Kernel
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <kernel/tty.h> /* term color functions and vga color defines */

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
        puts(s);                                            \
        term_setcol(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK); \
    }

/* test_libk: called by kernel_main to test libk functions */
static inline void test_libk() {
    char buf[255] = { 0 };

    TEST_TITLE("\nTesting colors, printf, and terminal scrolling...");
    for (int fg = 0; fg <= 15; fg++) {
        if (fg == VGA_COLOR_BLACK) {
            term_setcol(fg, VGA_COLOR_LIGHT_GREY);
            printf("%d", fg);
            term_setcol(fg, VGA_COLOR_BLACK);
            putchar(' ');
        } else {
            term_setcol(fg, VGA_COLOR_BLACK);
            printf("%d ", fg);
        }
    }

    TEST_TITLE("\n\nTesting stdlib.h, string.h and stdio.h functions...");

    printf("strlen(\"abcd\") -> %d\n", strlen("abcd"));
    printf("memcmp(\"abcd\", \"abca\", 4) -> %d\n", memcmp("abcd", "abc1", 4));
    printf("memcmp(\"abcd\", \"abce\", 4) -> %d\n", memcmp("abcd", "abce", 4));
    printf("memcmp(\"12345\", \"12345\", 5) -> %d\n", memcmp("12345", "12345", 5));

    /* More than one line for the null terminator */
    printf("memset(buf, 'h', 5) -> ");
    memset(buf, 'h', 5);
    buf[5] = '\0';
    puts(buf);

    printf("memcpy(&buf[5], &buf[0], 5) -> ");
    memcpy(&buf[5], &buf[0], 5);
    buf[10] = '\0';
    puts(buf);
}

/* kernel_main: Called by boot.asm */
void kernel_main() {
    term_init();

    term_setcol(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
    puts("Hello, welcome to the Free and Simple Operating System!\n"
         "This project is still being developed. For more information, see:");
    term_setcol(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
    puts("https://github.com/fs-os/fs-os");

    test_libk();
}

