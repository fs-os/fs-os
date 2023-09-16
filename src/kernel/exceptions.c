
#include <stdio.h>
#include <stdlib.h>
#include <kernel/exceptions.h>
#include <kernel/framebuffer_console.h> /* fbc_setfore() */
#include <kernel/color.h>               /* COLOR_* */

static char* exceptions[] = {
    [0]  = "division by zero",
    [1]  = "debug", /* Handled in handle_debug() */
    [2]  = "non maskable interrupt",
    [3]  = "breakpoint",
    [4]  = "overflow",
    [5]  = "bound range exceeded",
    [6]  = "invalid opcode",
    [7]  = "device not available",
    [8]  = "double fault",
    [10] = "invalid TSS",
    [11] = "segment not present",
    [12] = "stack exception",
    [13] = "general protection fault",
    [14] = "page fault",
    [15] = "reserved exception, something is very wrong", /* Reserved */
    [16] = "x87 floating point exception",
    [17] = "alignment check",
    [18] = "machine check",
    [19] = "SIMD floating point exception",
    [20] = "virtualization exception",
    [30] = "security exception",
};

void handle_exception(int code, void* eip) {
    panic(NULL, 0, "exception @ %p: %s\n", eip, exceptions[code]);
}

void handle_debug(uint64_t* tsc, void* eip) {
    static uint64_t last_tsc = 0;

    uint32_t old_fg, old_bg;
    fbc_getcols(&old_fg, &old_bg);

    fbc_setfore(COLOR_BLUE);
    putchar('[');
    fbc_setfore(COLOR_BLUE_B);
    printf("DbgException");
    fbc_setfore(COLOR_BLUE);
    printf("] Trap @ %p", eip);

    /* Not compiled with DEBUG, exc_debug() passes NULL */
    if (tsc != NULL) {
        printf(" TimeStampCounter: %llu", *tsc);

        /* FIXME: Currently broken (Compile with DEBUG defined) */
        if (last_tsc != 0)
            printf(" (+%llu)", *tsc - last_tsc);
    }

    putchar('\n');
    fbc_setfore(old_fg);

    /* Store last TSC at the bottom so we don't store time from this func */
    if (tsc != NULL)
        last_tsc = *tsc;
}
