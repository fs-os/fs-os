
#ifndef _KERNEL_MULTITASK_H
#define _KERNEL_MULTITASK_H

/* Task context struct */
typedef struct Ctx Ctx;
struct Ctx {
    Ctx* next;
    /* TODO */
};

/* Task state segment, loaded to the GDT */
typedef struct Tss Tss;
struct Tss {
    uint16_t link;
    uint16_t pad0; /* Padding */

    uint32_t esp0; /* Used for multitasking */
    uint16_t ss0;
    uint16_t pad1; /* Padding */
    uint32_t esp1;
    uint16_t ss1;
    uint16_t pad2; /* Padding */
    uint32_t esp2;
    uint16_t ss2;
    uint16_t pad3; /* Padding */

    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;

    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;

    uint16_t es;
    uint16_t pad4; /* Padding */
    uint16_t cs;
    uint16_t pad5; /* Padding */
    uint16_t ss;
    uint16_t pad6; /* Padding */
    uint16_t ds;
    uint16_t pad7; /* Padding */
    uint16_t fs;
    uint16_t pad8; /* Padding */
    uint16_t gs;
    uint16_t pad9; /* Padding */
    uint16_t ldtr;
    uint16_t pad10; /* Padding */
    uint16_t pad11; /* Padding (before iobp) */
    uint16_t iobp;
    uint32_t ssp;  /* 0x68 (0x68 - 0x6C) */
} __attribute__((packed));

#endif /* _KERNEL_MULTITASK_H */

