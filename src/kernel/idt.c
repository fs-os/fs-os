
/*
 * Interrupt Descriptor Table. For more information, see:
 *   https://wiki.osdev.org/Interrupt_Descriptor_Table
 *   http://www.brokenthorn.com/Resources/OSDevPic.html
 */

#include <stdint.h>
#include <stdlib.h>
#include <kernel/io.h>
#include <kernel/idt.h>
#include <kernel/exceptions.h>

#define IDT_SZ 256

/* Interrupt descriptor table itself, 256 entries */
idt_entry idt[IDT_SZ] = { 0 };

/* The descriptor containing the idt size and ptr. Initialized in idt_init */
idt_descriptor descriptor;

/* register_isr: registers an interrupt service routine in the selected idt idx */
static void register_isr(uint16_t idx, uint32_t func) {
    if (idx >= IDT_SZ)
        abort_line("Idx out of bounds when registering ISR.");

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

/* pic_remap: remap the programable interrupt controllers so the interrupt numbers of
 * the master PIC don't overlap with the CPU exceptions */
static inline void pic_remap(void) {
    /*
     * Modern computers have 2 PICs (Programable Interrupt Controller), the master
     * and the slave. One line of the master PIC is used to signal the slave PIC.
     * We are going to change the PIC interrupt numbers because by default, the
     * ranges are:
     *   IRQ 0..7  -> INT 0x08..0x0F
     *   IRQ 8..15 -> INT 0x70..0x77
     * The first 8 overlap with the CPU's exceptions (src/kernel/exceptions.c), so we
     * need to move them.
     * We comunicate through the IO bus. Similar to the RTC, each PIC has a command
     * port and a data port:
     *   Master -> cmd: 0x20, data: 0x21
     *   Slave  -> cmd: 0xA0, data: 0xA1
     * TODO: The flags need comments with explanation.
     */

    /* Start the initialization sequence in cascade mode */
    io_outb(PIC_MASTER_CMD, ICW1_INIT | ICW1_ICW4);
    io_outb(PIC_SLAVE_CMD, ICW1_INIT | ICW1_ICW4);

    /* Make the master PIC start in the interrupt number 32 instead of 8
     * Make the slave PIC start in the interrupt number 40 instead of 0x70 */
    io_outb(PIC_MASTER_DATA, 32);
    io_outb(PIC_SLAVE_DATA, 40);

    /* Slave PIC is chained to the master */
    io_outb(PIC_MASTER_DATA, 4);
    io_outb(PIC_SLAVE_DATA, 2);

    /* We are in 32 bits */
    io_outb(PIC_MASTER_DATA, ICW4_8086);
    io_outb(PIC_SLAVE_DATA, ICW4_8086);

    /* We don't need to restore saved masks because they are not masked */
    io_outb(PIC_MASTER_DATA, 0);
    io_outb(PIC_SLAVE_DATA, 0);
}

/* idt_init: initialize the idt and the idt descriptor */
void idt_init(void) {
    /* Descritor */
    descriptor.limit = (IDT_SZ * sizeof(idt_entry)) - 1;
    descriptor.base  = &idt[0];

    /* Remap the PICs so the interrupt numbers of the master PIC don't overlap with
     * the CPU exceptions. See comment inside function. */
    pic_remap();

    /* Exception Handling. exc_* defined in src/kernel/idt_asm.asm */
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

    /* IRQs */
    register_isr(32, (uint32_t)&irq_pit);  /* src/kernel/idt_asm.asm */
    /* register_isr(33, (void*)exc_33); */ /* TODO: Keyboard */

    /* Unused IRQs, just ignore. See src/kernel/idt_asm.asm */
    for (int i = 33; i < 40; i++)
        register_isr(i, (uint32_t)&irq_default_master);
    for (int i = 40; i < 48; i++)
        register_isr(i, (uint32_t)&irq_default_slave);

    /* src/kernel/idt_asm.asm */
    idt_load(&descriptor);

    /* Enable interrupts (afaik sti is the opposite of cli) */
    asm("sti");
}

