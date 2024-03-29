
#ifndef KERNEL_IDT_H_
#define KERNEL_IDT_H_ 1

#include <stdint.h>

/**
 * @def P_BIT
 * @brief Present bit
 *
 * 0x80, 0b10000000
 */
#define P_BIT (1 << 7)

/**
 * @enum idt_dpl
 * @brief Values for the DPL bit of the Gate Descriptor.
 */
enum idt_dpl {
    DPL_OFF = 0,
    DPL_ON  = 1,
};

/**
 * @enum idt_gate_types
 * @brief Gate type numbers for the IDT.
 * @details The 2 main types of ISR (Interrupt Service Routines) are traps and
 * interrupts. Traps are exceptions generated by errors, and store the address
 * of the current instruction. Interrupts are used to handle events that need
 * attention unrelated to the current instruction.
 *
 * For more information, see: https://wiki.osdev.org/IDT#Gate_Types
 */
enum idt_gate_types {
    IDT_GATE_TASK       = 0x5, /* Task gate. Unused */
    IDT_GATE_16BIT_INT  = 0x6, /* 16 bit interrupt. Unused */
    IDT_GATE_16BIT_TRAP = 0x7, /* 16 bit trap. Unused */
    IDT_GATE_32BIT_INT  = 0xE, /* 32 bit interrupt */
    IDT_GATE_32BIT_TRAP = 0xF, /* 32 bit trap */
};

/**
 * @enum pic_ports
 * @brief IO ports for the master and slave PICs.
 * @details PIC means Programmable Interrupt Controller
 *
 * See https://wiki.osdev.org/PIC
 */
enum pic_ports {
    PIC_MASTER_CMD  = 0x20, /* Command I/O port of the master PIC */
    PIC_MASTER_DATA = 0x21, /* Data I/O port of the master PIC */
    PIC_SLAVE_CMD   = 0xA0, /* Command I/O port of the slave PIC */
    PIC_SLAVE_DATA  = 0xA1, /* Data I/O port of the slave PIC */
};

/**
 * @enum pic_flags
 * @brief Flags used for the data and command pic_ports
 */
enum pic_flags {
    ICW1_ICW4      = 0x01, /* ICW4 (not) needed */
    ICW1_SINGLE    = 0x02, /* Single (cascade) mode */
    ICW1_INTERVAL4 = 0x04, /* Call address interval 4 (8) */
    ICW1_LEVEL     = 0x08, /* Level triggered (edge) mode */
    ICW1_INIT      = 0x10, /* Initialization - required! */

    ICW4_8086       = 0x01, /* 8086/88 (MCS-80/85) mode */
    ICW4_AUTO       = 0x02, /* Auto (normal) EOI */
    ICW4_BUF_SLAVE  = 0x08, /* Buffered mode/slave */
    ICW4_BUF_MASTER = 0x0C, /* Buffered mode/master */
    ICW4_SFNM       = 0x10, /* Special fully nested (not) */
};

/**
 * @struct idt_entry
 * @brief Entry structure of the IDT
 *
 * @details IDT means Interrupt Descriptor Table. This table will contain
 * entries that give information about the ISRs (Interrupt Service Routines) of
 * each interrupt number, plus some other flags and information.
 *
 * See https://wiki.osdev.org/IDT
 */
typedef struct {
    /** @brief Bits 0..15 of the offset. The offset is relative to the current
     * GDT segment. */
    uint16_t offset_l;

    /** @brief Code segment selector of the GDT (or LDT) */
    uint16_t selector;

    /** @brief Unused, 0 */
    uint8_t zero;

    /** @brief Gate Type, DPL (permissions) and Present flag */
    uint8_t type;

    /** @brief Bits 16..31 of the offset */
    uint16_t offset_h;
} __attribute__((packed)) idt_entry;

/**
 * @struct idt_descriptor
 * @brief Actual IDT descriptor structure needed by the LIDT asm instruction.
 * @details Same as the GDT descriptor from src/kernel/gdt.asm
 * */
typedef struct {
    /** @brief Size of the IDT minus 1 */
    uint16_t limit;

    /** @brief Address of the IDT itself */
    idt_entry* base;
} __attribute__((packed)) idt_descriptor;

/**
 * @brief Initialize the idt and the idt descriptor
 * @details Defined in src/kernel/idt.c
 */
void idt_init(void);

/**
 * @brief Loads the idt descriptor from assembly using the lidt instruction.
 * @details Defined in src/kernel/idt.asm
 * @param[in] idt_desc Pointer to the idt_descriptor structure
 */
void idt_load(void* idt_desc);

#endif /* KERNEL_IDT_H_ */
