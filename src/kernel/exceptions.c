
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
static void handle_exception(int exc) {
    /* See https://gcc.gnu.org/onlinedocs/gcc/Using-Assembly-Language-with-C.html */
    asm("cli");

    abort("exception: %s\n", exceptions[exc]);
}

/* exc_X: call the exception handler with the specified IRQ. Used as offsets for the
 * idt. */
void exc_0(void) {
    handle_exception(0);
}
void exc_1(void) {
    handle_exception(1);
}
void exc_2(void) {
    handle_exception(2);
}
void exc_3(void) {
    handle_exception(3);
}
void exc_4(void) {
    handle_exception(4);
}
void exc_5(void) {
    handle_exception(5);
}
void exc_6(void) {
    handle_exception(6);
}
void exc_7(void) {
    handle_exception(7);
}
void exc_8(void) {
    handle_exception(8);
}
void exc_10(void) {
    handle_exception(10);
}
void exc_11(void) {
    handle_exception(11);
}
void exc_12(void) {
    handle_exception(12);
}
void exc_13(void) {
    handle_exception(13);
}
void exc_14(void) {
    handle_exception(14);
}
void exc_15(void) {
    handle_exception(15);
}
void exc_16(void) {
    handle_exception(16);
}
void exc_17(void) {
    handle_exception(17);
}
void exc_18(void) {
    handle_exception(18);
}
void exc_19(void) {
    handle_exception(19);
}
void exc_20(void) {
    handle_exception(20);
}
void exc_30(void) {
    handle_exception(30);
}

