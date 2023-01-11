
#include <stdint.h>
#include <stdlib.h>
#include <kernel/idt.h>
#include <kernel/misc_asm.h>
#include <kernel/exceptions.h>

#define IDT_SZ 256

/* Interrupt Descriptor Table. For more information, see:
 *   https://wiki.osdev.org/Interrupt_Descriptor_Table */
idt_entry idt[IDT_SZ] = { 0 };

/* The descriptor containing the idt size and ptr. Initialized in idt_init */
idt_descriptor descriptor;

/* register_isr: registers an interrupt service routine in the selected idt idx */
static void register_isr(uint16_t idx, uint32_t func) {
    if (idx >= IDT_SZ)
        abort("Idx out of bounds when registering ISR.");

    idt[idx] = (idt_entry){
        .selector = 0x8, /* 00000000 00001000. Last 3 bits of the selector are TI and
                            RPL. We only want to set the idx to 1 (first idx is the
                            null gdt entry) */
        .offset_l = ((uint32_t)func) & 0xFFFF,
        .offset_h = (((uint32_t)func) >> 16) & 0xFFFF,
        .type     = P_BIT | DPL_NONE | IDT_GATE_32BIT_INT,
        .zero     = 0,
    };
}

/* idt_init: initialize the idt and the idt descriptor */
void idt_init(void) {
    /* Descritor */
    descriptor.limit = (IDT_SZ * sizeof(idt_entry)) - 1;
    descriptor.base  = idt;

    /* Exception Handling */
    register_isr(0, (uint32_t)&exc_0);
    register_isr(1, (uint32_t)&exc_1);
    register_isr(2, (uint32_t)&exc_2);
    register_isr(3, (uint32_t)&exc_3);
    register_isr(4, (uint32_t)&exc_4);
    register_isr(5, (uint32_t)&exc_5);
    register_isr(6, (uint32_t)&exc_6);
    register_isr(7, (uint32_t)&exc_7);
    register_isr(8, (uint32_t)&exc_8);
    register_isr(10, (uint32_t)&exc_10);
    register_isr(11, (uint32_t)&exc_11);
    register_isr(12, (uint32_t)&exc_12);
    register_isr(13, (uint32_t)&exc_13);
    register_isr(14, (uint32_t)&exc_14);
    register_isr(15, (uint32_t)&exc_15);
    register_isr(16, (uint32_t)&exc_16);
    register_isr(17, (uint32_t)&exc_17);
    register_isr(18, (uint32_t)&exc_18);
    register_isr(19, (uint32_t)&exc_19);
    register_isr(20, (uint32_t)&exc_20);
    register_isr(30, (uint32_t)&exc_30);
    /* register_isr(32, (void*)exc_32); */ /* TODO: PIT */
    /* register_isr(33, (void*)exc_33); */ /* TODO: Keyboard */

    /* src/kernel/misc.asm */
    load_idt(&descriptor);

    /* FIXME */
    /* asm_sti(); */
}

