
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <kernel/paging.h>              /* paging_init */
#include <kernel/heap.h>                /* heap_init */
#include <kernel/vga.h>                 /* vga_init, vga_sprint */
#include <kernel/framebuffer.h>         /* fb_init, fb_setpx */
#include <kernel/framebuffer_console.h> /* fbc_init */
#include <kernel/idt.h>                 /* idt_init */
#include <kernel/pit.h>                 /* pit_init */
#include <kernel/rand.h>                /* check_rand */
#include <kernel/rtc.h>                 /* rtc_get_datetime */
#include <kernel/pcspkr.h>              /* pcspkr_beep */
#include <kernel/keyboard.h>            /* kb_setlayout, kb_getchar_init */
#include <kernel/multitask.h>           /* mt_init */

#include <kernel/multiboot.h> /* multiboot info structure */
#include <fonts/main_font.h>

#include "../apps/sh/sh.h" /* sh_main */

#include "../media/logo_small.h"

#if defined(__linux__)
#error "You are not using a cross compiler." \
    "For more information see: https://github.com/fs-os/cross-compiler"
#endif

#if !defined(__i386__)
#error "You are not using a ix86-elf compiler." \
    "For more information see: https://github.com/fs-os/cross-compiler"
#endif

#define TEST_TITLE(s)               \
    {                               \
        fbc_setfore(COLOR_WHITE_B); \
        puts(s);                    \
        fbc_setfore(COLOR_GRAY);    \
    }

#define LOAD_INFO(s)                  \
    {                                 \
        fbc_setfore(COLOR_MAGENTA_B); \
        printf(" * ");                \
        fbc_setfore(COLOR_MAGENTA);   \
        puts(s);                      \
        fbc_setfore(COLOR_WHITE);     \
    }

#define LOAD_IGNORE(s)             \
    {                              \
        fbc_setfore(COLOR_GRAY_B); \
        printf(" * ");             \
        puts(s);                   \
        fbc_setfore(COLOR_WHITE);  \
    }

#define LOAD_ERROR(s)             \
    {                             \
        fbc_setfore(COLOR_RED_B); \
        printf(" * ");            \
        fbc_setfore(COLOR_RED);   \
        puts(s);                  \
        fbc_setfore(COLOR_WHITE); \
    }

#define SYSTEM_INFO(s1, s2fmt, ...) \
    {                               \
        fbc_setfore(COLOR_WHITE_B); \
        printf("\t%s", s1);         \
        fbc_setfore(COLOR_WHITE);   \
        printf(s2fmt, __VA_ARGS__); \
        putchar('\n');              \
    }

/* Default layout, declared in keyboard.c */
extern Layout us_layout;

static inline void test_colors(void) {
    printf("\n\t");
    fbc_setfore(COLOR_BLACK);
    printf("%c%c%c", 219, 219, 219);
    fbc_setfore(COLOR_RED);
    printf("%c%c%c", 219, 219, 219);
    fbc_setfore(COLOR_GREEN);
    printf("%c%c%c", 219, 219, 219);
    fbc_setfore(COLOR_YELLOW);
    printf("%c%c%c", 219, 219, 219);
    fbc_setfore(COLOR_BLUE);
    printf("%c%c%c", 219, 219, 219);
    fbc_setfore(COLOR_MAGENTA);
    printf("%c%c%c", 219, 219, 219);
    fbc_setfore(COLOR_CYAN);
    printf("%c%c%c", 219, 219, 219);
    fbc_setfore(COLOR_GRAY);
    printf("%c%c%c", 219, 219, 219);
    fbc_setfore(COLOR_WHITE);
    printf("%c%c%c", 219, 219, 219);
    printf("\n\t");

    fbc_setfore(COLOR_BLACK_B);
    printf("%c%c%c", 219, 219, 219);
    fbc_setfore(COLOR_RED_B);
    printf("%c%c%c", 219, 219, 219);
    fbc_setfore(COLOR_GREEN_B);
    printf("%c%c%c", 219, 219, 219);
    fbc_setfore(COLOR_YELLOW_B);
    printf("%c%c%c", 219, 219, 219);
    fbc_setfore(COLOR_BLUE_B);
    printf("%c%c%c", 219, 219, 219);
    fbc_setfore(COLOR_MAGENTA_B);
    printf("%c%c%c", 219, 219, 219);
    fbc_setfore(COLOR_CYAN_B);
    printf("%c%c%c", 219, 219, 219);
    fbc_setfore(COLOR_GRAY_B);
    printf("%c%c%c", 219, 219, 219);
    fbc_setfore(COLOR_WHITE_B);
    printf("%c%c%c", 219, 219, 219);
    printf("\n\t");

    fbc_setfore(COLOR_WHITE);
}

/**
 * @brief Prints the OS logo using the GIMP macro
 * @param ypad Top padding in px
 * @param xpad Left padding in px
 */
static void print_logo(unsigned int ypad, unsigned int xpad) {
    char rgb[3] = { 0 };
    /* Copy ptr because HEADER_PIXEL increases it */
    char* logo_ptr = fsos_logo_s;

    for (unsigned int y = 0; y < fsos_logo_s_h; y++) {
        for (unsigned int x = 0; x < fsos_logo_s_w; x++) {
            HEADER_PIXEL(logo_ptr, rgb);
            fb_setpx(y + ypad, x + xpad, rgb[0], rgb[1], rgb[2]);
        }
    }
}

/**
 * @brief C entry point of the kernel. Called by boot.asm
 * @param mb_info Pointer to the Multiboot information struct from the
 * booloader.
 */
void kernel_main(Multiboot* mb_info) {
    idt_init();
    paging_init();
    heap_init();

    /* Currently unused */
    vga_init();
    vga_sprint("VGA terminal initialized.\n");

    if (mb_info->framebuffer_type != FB_TYPE_RGB) {
        vga_sprint("Could not initialize framebuffer on RGB mode.\n");
        abort();
    }

    mt_init();

    fb_init((uint32_t*)(uint32_t)mb_info->framebuffer_addr,
            mb_info->framebuffer_pitch, mb_info->framebuffer_width,
            mb_info->framebuffer_height, mb_info->framebuffer_bpp);
    vga_sprint("Framebuffer initialized.\n");

    print_logo(5, 0);
    print_logo(5, 100);
    print_logo(5, 200);

    fbc_init(110, 3, mb_info->framebuffer_height - 110 - 5,
             mb_info->framebuffer_width - 3 * 2, &main_font);

    /* Once we have a framebuffer terminal, print previous messages too */
    LOAD_INFO("IDT initialized.");
    LOAD_INFO("Paging initialized.");
    LOAD_INFO("Heap initialized.");
    LOAD_INFO("Multitasking initialized.");
    LOAD_INFO("Framebuffer initialized.");
    LOAD_INFO("Framebuffer console initialized.");

    /* Init PIT with 1ms interval (1/1000 of a sec) */
    pit_init(1000);
    LOAD_INFO("PIT initialized.");

    if (check_rdseed()) {
        LOAD_INFO("RDSEED supported.");
    } else {
        LOAD_IGNORE("RDSEED not supported.");
    }

    if (check_rdrand()) {
        LOAD_INFO("RDRAND supported.");
    } else {
        LOAD_IGNORE("RDRAND not supported.");
    }

    kb_setlayout(&us_layout);
    kb_getchar_init();
    LOAD_INFO("Keyboard initialized.");
    putchar('\n');

    LOAD_INFO("System info:");
    SYSTEM_INFO("Memory:\t\t", "%ldMiB", mb_info->mem_upper / 1024);
    SYSTEM_INFO("Resolution:\t", "%ldx%ld", mb_info->framebuffer_width,
                mb_info->framebuffer_height);
    SYSTEM_INFO("Font:\t\t", "%s", main_font.name);
    DateTime now = rtc_get_datetime();
    SYSTEM_INFO("Time:\t\t", "%2d/%2d/%2d - %2d:%2d:%2d", now.date.d,
                now.date.m, now.date.y, now.time.h, now.time.m, now.time.s);
    putchar('\n');

    LOAD_INFO("Color palette:");
    test_colors();

    /* ---------------------------------------------------------------------- */

    fbc_setfore(COLOR_MAGENTA);
    puts("\nHello, welcome to the Free and Simple Operating System!\n"
         "This project is still being developed. For more information, see:");
    fbc_setfore(COLOR_GREEN);
    puts("https://github.com/fs-os/fs-os");
    fbc_setfore(COLOR_WHITE);

    /* Main shell */
    sh_main();

    for (;;)
        asm("hlt");

    __builtin_unreachable();
}

