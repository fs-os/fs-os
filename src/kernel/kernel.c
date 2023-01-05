
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

#include <kernel/alloc.h>               /* init_heap */
#include <kernel/vga.h>                 /* init_term */
#include <kernel/framebuffer.h>         /* fb_init, fb_setpx */
#include <kernel/framebuffer_console.h> /* fbc_init */

#include <kernel/multiboot.h> /* multiboot info structure */
#include <fonts/main_font.h>

#include "logo_small.h"

#if defined(__linux__)
#error "You are not using a cross compiler." \
    "For more information see: https://github.com/fs-os/cross-compiler"
#endif

#if !defined(__i386__)
#error "You are not using a ix86-elf compiler." \
    "For more information see: https://github.com/fs-os/cross-compiler"
#endif

/* TODO: fbc colors */
#define TEST_TITLE(s) \
    { puts(s); }

/* test_libk: called by kernel_main to test libk functions */
static inline void test_libk(void) {
    char buf[255] = { 0 };

    /* TODO: fbc color and scrolling test */

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

/* print_logo: prints the logo from logo.h using the GIMP macro */
void print_logo(unsigned int ypad, unsigned int xpad) {
    char rgb[3]      = { 0 };
    char* logo_start = fsos_logo_s;

    for (unsigned int y = 0; y < fsos_logo_s_h; y++) {
        for (unsigned int x = 0; x < fsos_logo_s_w; x++) {
            HEADER_PIXEL(fsos_logo_s, rgb);
            fb_setpx(y + ypad, x + xpad, rgb[0], rgb[1], rgb[2]);
        }
    }

    /* Reset ptr because HEADER_PIXEL increases it */
    fsos_logo_s = logo_start;
}

/* kernel_main: Called by boot.asm */
void kernel_main(Multiboot* mb_info) {
    init_heap();

    /* Currently unused */
    term_init();
    term_setcol(VGA_COLOR_GREEN, VGA_COLOR_RED);
    term_sprint("VGA terminal initialized.\n");

    if (mb_info->framebuffer_type != FB_TYPE_RGB) {
        term_sprint("Could not initialize framebuffer on RGB mode.\n");
        abort(NULL);
    }

    fb_init((uint32_t*)(uint32_t)mb_info->framebuffer_addr,
            mb_info->framebuffer_pitch, mb_info->framebuffer_width,
            mb_info->framebuffer_height, mb_info->framebuffer_bpp);
    term_sprint("Framebuffer initialized.\n");

    print_logo(5, 0);
    print_logo(5, 100);
    print_logo(5, 200);

    fbc_init(110, 3, mb_info->framebuffer_height - 110 - 5,
             mb_info->framebuffer_width - 3 * 2, &main_font);
    puts("Framebuffer console initialized.");

    for (int i = 0; i < 10; i++)
        puts("!\"#$%&\'()*+,-./"
             "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
             "abcdefghijklmnopqrstuvwxyz{|}~");

    puts("Hello, welcome to the Free and Simple Operating System!\n"
         "This project is still being developed. For more information, see:");
    puts("https://github.com/fs-os/fs-os");

    TEST_TITLE("\nMultiboot info");
    printf("mem_lower: %d\n"
           "mem_upper: %d\n"
           "fb_pitch: %d\n"
           "fb_width: %d\n"
           "fb_height: %d\n"
           "fb_bpp: %d\n"
           "fb_type: %d\n",
           mb_info->mem_lower, mb_info->mem_upper, mb_info->framebuffer_pitch,
           mb_info->framebuffer_width, mb_info->framebuffer_height,
           mb_info->framebuffer_bpp, mb_info->framebuffer_type);

    test_libk();
}

