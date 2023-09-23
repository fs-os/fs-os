
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <kernel/util.h>

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

#include <kernel/multiboot.h> /* Multiboot info structure */
#include <fonts/main_font.h>

#include "../apps/sh/sh.h" /* sh_main */

#include "../media/logo_small.h"

#if defined(__linux__)
#error "You are not using a cross compiler." \
    "For more information see: https://github.com/fs-os/cross-compiler"
#endif

#if !defined(__i386__)
#error "You are not using a i386-elf compiler." \
    "For more information see: https://github.com/fs-os/cross-compiler"
#endif

#define TEST_TITLE(...)         \
    fbc_setfore(COLOR_WHITE_B); \
    printf(__VA_ARGS__);        \
    putchar('\n');              \
    fbc_setfore(COLOR_GRAY);

#define LOAD_INFO(...)            \
    fbc_setfore(COLOR_MAGENTA_B); \
    printf(" * ");                \
    fbc_setfore(COLOR_MAGENTA);   \
    printf(__VA_ARGS__);          \
    putchar('\n');                \
    fbc_setfore(COLOR_WHITE);

#define LOAD_IGNORE(...)       \
    fbc_setfore(COLOR_GRAY_B); \
    printf(" * ");             \
    printf(__VA_ARGS__);       \
    putchar('\n');             \
    fbc_setfore(COLOR_WHITE);

#define LOAD_ERROR(...)       \
    fbc_setfore(COLOR_RED_B); \
    printf(" * ");            \
    fbc_setfore(COLOR_RED);   \
    printf(__VA_ARGS__);      \
    putchar('\n');            \
    fbc_setfore(COLOR_WHITE);

#define SYSTEM_INFO(s1, ...)    \
    fbc_setfore(COLOR_WHITE_B); \
    printf("\t%s", s1);         \
    fbc_setfore(COLOR_WHITE);   \
    printf(__VA_ARGS__);        \
    putchar('\n');

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
    printf("\n");

    fbc_setfore(COLOR_WHITE);
}

/* Start and end of kernel, declared in cfg/linker.ld */
extern uint8_t _start;
extern uint8_t _bss_end;

/* If false, this machine doesn't support SSE, and it's enabled. SSE/SSE2
 * support is checked in src/kernel/boot.asm if ENABLE_SSE is defined. */
bool sse_supported = true;

/* If false, this machine doesn't support MSR. This should only matter if DEBUG
 * is defined (i.e. we are going to use hardware debug functionalities). MSR
 * support is checked in src/kernel/boot.asm if DEBUG is defined. */
bool msr_supported = true;

/* If false, this machine doesn't support the Time-Stamp Counter (TSC). This
 * should only matter if DEBUG is defined (i.e. we are going to use hardware
 * debug functionalities). TSC support is checked in src/kernel/boot.asm if
 * DEBUG is defined. */
bool tsc_supported = true;

/**
 * @brief C entry point of the kernel. Called by boot.asm
 * @param mb_info Pointer to the Multiboot information struct from the
 * bootloader.
 */
void kernel_main(Multiboot* mb_info) {
    idt_init();
    paging_init();
    heap_init();

    /* Currently unused */
    vga_init();
    vga_print("VGA terminal initialized.\n");

    if (mb_info->framebuffer_type != FB_TYPE_RGB) {
        vga_setcol(VGA_COLOR_RED, VGA_COLOR_BLACK);
        vga_print("Error. Could not initialize framebuffer on RGB mode.\n");
        abort();
    }

    mt_init();

    fb_init((uint32_t*)(uint32_t)mb_info->framebuffer_addr,
            mb_info->framebuffer_pitch, mb_info->framebuffer_width,
            mb_info->framebuffer_height, mb_info->framebuffer_bpp);
    vga_print("Framebuffer initialized.\n");

    /* Draw the 3 logos on top */
    const uint32_t logo_y = 3;
    const uint32_t logo_x = 3;
    const uint32_t logo_h = fsos_logo_s.h;
    const uint32_t logo_w = fsos_logo_s.w;
    fb_drawimage(logo_y, logo_x + (logo_w * 0), &fsos_logo_s);
    fb_drawimage(logo_y, logo_x + (logo_w * 1), &fsos_logo_s);
    fb_drawimage(logo_y, logo_x + (logo_w * 2), &fsos_logo_s);

    /* Get framebuffer console pos and size and initialize it */
    const uint32_t fbc_margin = 3;
    const uint32_t fbc_y      = logo_y + logo_h + fbc_margin;
    const uint32_t fbc_x      = fbc_margin;
    const uint32_t fbc_h      = fb_get_height() - fbc_y - fbc_margin;
    const uint32_t fbc_w      = fb_get_width() - (fbc_margin * 2);
    fbc_init(fbc_y, fbc_x, fbc_h, fbc_w, &main_font);

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

    if (!check_rdseed()) {
        LOAD_IGNORE("RDSEED not supported.");
    }

    if (!check_rdrand()) {
        LOAD_IGNORE("RDRAND not supported.");
    }

    if (!sse_supported) {
        LOAD_ERROR("SSE/SSE2 not supported on this machine. Please re-compile "
                   "with DISABLE_SSE=true defined in config.mk");
        abort();
    }

    if (!msr_supported) {
        LOAD_ERROR("MSR is not supported on this machine. Please re-compile "
                   "without DEBUG defined.");
        abort();
    }

    if (!tsc_supported) {
        LOAD_ERROR("Time-Stamp Counter (TSC) is not supported on this machine. "
                   "Please re-compile without DEBUG defined.");
        abort();
    }

    kb_setlayout(&us_layout);
    kb_getchar_init();
    LOAD_INFO("Keyboard initialized.");
    putchar('\n');

    LOAD_INFO("System info:");
    SYSTEM_INFO("Kernel:\t\t", "%p - %p", &_start, &_bss_end);
    SYSTEM_INFO("Memory:\t\t", "%ldMiB", mb_info->mem_upper / 1024);
    SYSTEM_INFO("Resolution:\t", "%ldx%ld", mb_info->framebuffer_width,
                mb_info->framebuffer_height);
    SYSTEM_INFO("Font:\t\t", main_font.name);

    /* "00/00/00 - 00:00:00" */
    const DateTime now = rtc_get_datetime();
    SYSTEM_INFO("Time:\t\t", "%02u/%02u/%02u - %02u:%02u:%02u\n", now.date.d,
                now.date.m, now.date.y, now.time.h, now.time.m, now.time.s);
    putchar('\n');

    LOAD_INFO("Color palette:");
    test_colors();

    /*------------------------------------------------------------------------*/

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
