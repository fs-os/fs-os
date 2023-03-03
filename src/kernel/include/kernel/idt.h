
#ifndef _KERNEL_IDT_H
#define _KERNEL_IDT_H

#define P_BIT    (1 << 7) /* 0x80, 0b10000000 */
#define DPL_NONE 0

enum idt_gate_types {
    IDT_GATE_TASK       = 0x5,
    IDT_GATE_16BIT_INT  = 0x6,
    IDT_GATE_16BIT_TRAP = 0x7,
    IDT_GATE_32BIT_INT  = 0xE,
    IDT_GATE_32BIT_TRAP = 0xF,
};

/* IO ports for the master and slave PICs. See https://wiki.osdev.org/PIC */
enum pic_ports {
    PIC_MASTER_CMD  = 0x20,
    PIC_MASTER_DATA = 0x21,
    PIC_SLAVE_CMD   = 0xA0,
    PIC_SLAVE_DATA  = 0xA1,
};

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

typedef struct {
    uint16_t offset_l; /* Bits 0..15 of the offset inside the segment */
    uint16_t selector; /* Code segment selector of the GDT (or LDT) */
    uint8_t zero;      /* Unused, 0 */
    uint8_t type;      /* Gate type, DPL (permissions), "Present" field */
    uint16_t offset_h; /* Bits 16..31 of the offset */
} __attribute__((packed)) idt_entry;

/* Same as the gdt descriptor from src/kernel/gdt.asm */
typedef struct {
    uint16_t limit;
    idt_entry* base;
} __attribute__((packed)) idt_descriptor;

/* idt_init: initialize the idt and the idt descriptor */
void idt_init(void);

/* idt_load: loads the idt descriptor from the assembly using the lidt
 * instruction */
void idt_load(void* idt_desc);

#endif /* _KERNEL_IDT_H */

