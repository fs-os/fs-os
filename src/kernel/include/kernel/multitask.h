
#ifndef _KERNEL_MULTITASK_H
#define _KERNEL_MULTITASK_H

#include <stdint.h>

typedef struct Ctx Ctx;

/**
 * @struct Task context struct
 * @details We could add more stuff like parent task and priority
 */
struct Ctx {
    Ctx* next;      /**< @brief Pointer to next task */
    Ctx* prev;      /**< @brief Pointer to next task */
    uint32_t stack; /**< @brief Pointer to the allocated stack for the task */
    uint32_t esp;   /**< @brief Stack top */
    uint32_t cr3;   /**< @brief cr3 register (page directory) */
    char* name;     /**< @brief Task name */
};

typedef struct tss_t Tss;

/**
 * @struct tss_t
 * @brief Task state segment, loaded to the GDT
 */
struct tss_t {
    uint16_t link;
    uint16_t pad0; /**< @brief Padding */

    uint32_t esp0; /**< @brief Used for multitasking */
    uint16_t ss0;
    uint16_t pad1; /**< @brief Padding */
    uint32_t esp1;
    uint16_t ss1;
    uint16_t pad2; /**< @brief Padding */
    uint32_t esp2;
    uint16_t ss2;
    uint16_t pad3; /**< @brief Padding */

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
    uint16_t pad4; /**< @brief Padding */
    uint16_t cs;
    uint16_t pad5; /**< @brief Padding */
    uint16_t ss;
    uint16_t pad6; /**< @brief Padding */
    uint16_t ds;
    uint16_t pad7; /**< @brief Padding */
    uint16_t fs;
    uint16_t pad8; /**< @brief Padding */
    uint16_t gs;
    uint16_t pad9; /**< @brief Padding */

    uint16_t ldtr;
    uint16_t pad10; /**< @brief Padding */
    uint16_t pad11; /**< @brief Padding (before iobp) */
    uint16_t iobp;
    uint32_t ssp; /**< @brief 0x68 (0x68 - 0x6C) */
} __attribute__((packed));

/**
 * @var mt_current_task
 * @brief Pointer to the current task (context) being used.
 * @details Defined in: src/kernel/multitask.h
 */
extern Ctx* mt_current_task;

/**
 * @brief Returns a pointer to the current task.
 * @details Defined in: src/kernel/gdt.asm
 * @return Pointer to the current Tss struct.
 */
Tss* tss_getptr(void);

/**
 * @brief Initialize multitasking.
 * @details Creates the first task for the kernel. Defined in
 * src/kernel/multitask.asm
 */
void mt_init(void);

/* mt_newtask:  */

/**
 * @brief Creates a new task named `name`, and with the entry point `entry`.
 * @details Defined in src/kernel/multitask.asm
 * @param[in] name The name of the new task.
 * @param[in] entry The entry point of the new task.
 * @return Pointer to the new task's context struct (Ctx).
 */
Ctx* mt_newtask(const char* name, void* entry);

/**
 * @brief Switch to task `next`
 * @details Defined in src/kernel/multitask.asm
 * @param[in] next New task context to switch to.
 */
void mt_switch(Ctx* next);

/**
 * @brief Frees the stack and ends the task passed as parameter.
 * @details The task should not be the current working task.
 * @param[out] task Task to kill.
 */
void mt_endtask(Ctx* task);

/**
 * @brief Returns a pointer to the current task context struct being used.
 * @details Defined in src/kernel/multitask.asm
 * @return Returns the current task's context struct (Ctx).
 */
Ctx* mt_gettask(void);

/**
 * @brief Print the list of tasks starting with the current one.
 * @details Defined in src/kernel/multitask.c
 */
void dump_task_list(void);

#endif /* _KERNEL_MULTITASK_H */
