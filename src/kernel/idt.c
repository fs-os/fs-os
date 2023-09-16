
/*
 * Interrupt Descriptor Table. For more information, see:
 *   https://wiki.osdev.org/Interrupt_Descriptor_Table
 *   http://www.brokenthorn.com/Resources/OSDevPic.html
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <kernel/io.h>
#include <kernel/idt.h>
#include <kernel/exceptions.h>

#define IDT_SZ 256

/** @brief Interrupt descriptor table itself, 256 entries. */
static idt_entry idt[IDT_SZ] = { 0 };

/** @brief The descriptor containing the idt size and ptr. Initialized in
 * idt_init. */
static idt_descriptor descriptor;

/**
 * @brief Registers an interrupt service routine in the selected index of the
 * idt array.
 * @param idx Index of the idt array.
 * @param func Pointer to ISR function.
 * @param trap_gate If true, the current gate type will be TRAP instead of INT.
 */
static void register_isr(uint16_t idx, void* func, bool trap_gate) {
    if (idx >= IDT_SZ)
        panic_line("Idx out of bounds when registering ISR.");

    const int gate_type = trap_gate ? IDT_GATE_32BIT_TRAP : IDT_GATE_32BIT_INT;

    idt[idx] = (idt_entry){
        .selector = 0x8, /* 00000000 00001000. Last 3 bits of the selector are
                            TI and RPL. We only want to set the idx to 1 (first
                            idx is the null gdt entry) */
        .offset_l = (uint32_t)func & 0xFFFF,
        .offset_h = ((uint32_t)func >> 16) & 0xFFFF,
        .type     = P_BIT | DPL_OFF | gate_type,
        .zero     = 0,
    };
}

/**
 * @brief Remap the programmable interrupt controllers so the interrupt numbers
 * of the master PIC don't overlap with the CPU exceptions.
 *
 * See wiki for more details.
 */
static inline void pic_remap(void) {
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

void idt_init(void) {
    /* Descriptor */
    descriptor.limit = (IDT_SZ * sizeof(idt_entry)) - 1;
    descriptor.base  = &idt[0];

    /* Remap the PICs so the interrupt numbers of the master PIC don't overlap
     * with the CPU exceptions. See comment inside function. */
    pic_remap();

    /* Exception Handling. With exceptions, we set the gate type to TRAP instead
     * of INT. The exc_N functions defined in src/kernel/idt.asm */
    register_isr(0, exc_0, true);
    register_isr(1, exc_debug, false); /* Special handler for debug */
    register_isr(2, exc_2, true);
    register_isr(3, exc_3, true);
    register_isr(4, exc_4, true);
    register_isr(5, exc_5, true);
    register_isr(6, exc_6, true);
    register_isr(7, exc_7, true);
    register_isr(8, exc_8, true);
    register_isr(10, exc_10, true);
    register_isr(11, exc_11, true);
    register_isr(12, exc_12, true);
    register_isr(13, exc_13, true);
    register_isr(14, exc_14, true);
    register_isr(15, exc_15, true); /* Reserved */
    register_isr(16, exc_16, true);
    register_isr(17, exc_17, true);
    register_isr(18, exc_18, true);
    register_isr(19, exc_19, true);
    register_isr(20, exc_20, true);
    register_isr(30, exc_30, true);

    /* IRQs. See src/kernel/idt.asm */
    register_isr(32, irq_pit, false); /* PIT. IRQ 0 */
    register_isr(33, irq_kb, false);  /* Keyboard. IRQ 1 */

    /* Unused IRQs, just ignore. See src/kernel/idt.asm */
    for (int i = 34; i < 40; i++)
        register_isr(i, irq_default_master, false);

    for (int i = 40; i < 48; i++)
        register_isr(i, irq_default_slave, false);

    /* See src/kernel/idt.asm */
    idt_load(&descriptor);

    /* Enable interrupts. It should not be necessary */
    asm("sti");
}
