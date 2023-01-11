
#include <stdlib.h>
#include <kernel/exceptions.h>
#include <kernel/misc_asm.h>

static char* exceptions[] = {
    [0]  = "Division by Zero",
    [1]  = "Debug",
    [2]  = "Non Maskable Interrupt",
    [3]  = "Breakpoint",
    [4]  = "Overflow",
    [5]  = "Bound Range Exceeded",
    [6]  = "Invalid opcode",
    [7]  = "Device not available",
    [8]  = "Double Fault",
    [10] = "Invalid TSS",
    [11] = "Segment not present",
    [12] = "Stack Exception",
    [13] = "General Protection fault",
    [14] = "Page fault",
    [16] = "x87 Floating Point Exception",
    [17] = "Alignment check",
    [18] = "Machine check",
    [19] = "SIMD floating point Exception",
    [20] = "Virtualization Exception",
    [30] = "Security Exception",
};

/* handle_exception: disables interrupts and panics with the specified exception. */
static void handle_exception(int exc) {
    asm_cli();
    abort(exceptions[exc]);
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

