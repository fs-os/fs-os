
#ifndef _KERNEL_MULTITASK_H
#define _KERNEL_MULTITASK_H

#include <stdint.h>

/* Task context struct */
typedef struct Ctx Ctx;
struct Ctx {
    Ctx* next;      /* Pointer to next task */
    Ctx* prev;      /* Pointer to next task */
    uint32_t stack; /* Pointer to the allocated stack for the task */
    uint32_t esp;   /* Stack top */
    uint32_t cr3;   /* cr3 register (page directory) */
    uint32_t state;
    char* name; /* Task name */

    /* We could add more stuff like parent task and priority */
};

/* Task state segment, loaded to the GDT */
typedef struct tss_t Tss;
struct tss_t {
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
    uint32_t ssp; /* 0x68 (0x68 - 0x6C) */
} __attribute__((packed));

/* Pointer to the current task (context) being used. Defined in:
 * src/kernel/multitask.h */
extern Ctx* mt_current_task;

/* tss_getptr: returns a pointer to a Tss struct. Defined in:
 * src/kernel/gdt.asm */
Tss* tss_getptr(void);

/* mt_init: initialize multitasking. Creates the first task for the kernel.
 * Defined in src/kernel/multitask.asm */
void mt_init(void);

/* mt_newtask: creates a new task named "name", and with the entry point
 * "entry". Defined in src/kernel/multitask.asm */
Ctx* mt_newtask(const char* name, void* entry);

/* mt_switch: switch to task "next". Defined in src/kernel/multitask.asm */
void mt_switch(Ctx* next);

/* mt_endtask: frees the stack and ends the task passed as parameter. The task
 * should not be the current working task.*/
void mt_endtask(Ctx* task);

/* mt_gettask: returns a pointer to the current task context struct being used.
 * Defined in src/kernel/multitask.asm */
Ctx* mt_gettask(void);

/* dump_task_list: print the list of tasks starting with the current one.
 * Defined in src/kernel/multitask.c */
void dump_task_list(void);

#endif /* _KERNEL_MULTITASK_H */

