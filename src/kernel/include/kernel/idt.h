
#ifndef _KERNEL_IDT_H
#define _KERNEL_IDT_H

#define P_BIT    (1 << 7) /* 0x80, 0b10000000 */
#define DPL_NONE 0

enum gate_types {
    IDT_GATE_TASK       = 0x5,
    IDT_GATE_16BIT_INT  = 0x6,
    IDT_GATE_16BIT_TRAP = 0x7,
    IDT_GATE_32BIT_INT  = 0xE,
    IDT_GATE_32BIT_TRAP = 0xF,
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

#endif /* _KERNEL_IDT_H */

