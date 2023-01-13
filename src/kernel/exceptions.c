
#include <stdlib.h>
#include <kernel/idt.h> /* asm_cli */
#include <kernel/exceptions.h>

static char* exceptions[] = {
    [0]  = "division by zero",
    [1]  = "debug",
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
    [16] = "x87 floating point exception",
    [17] = "alignment check",
    [18] = "machine check",
    [19] = "SIMD floating point exception",
    [20] = "virtualization exception",
    [30] = "security exception",
};

/* handle_exception: disables interrupts and panics with the specified exception. */
void handle_exception(int exc) {
    /* See https://gcc.gnu.org/onlinedocs/gcc/Using-Assembly-Language-with-C.html */
    asm("cli");

    abort("exception: %s\n", exceptions[exc]);
}

