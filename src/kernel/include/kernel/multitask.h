
#ifndef KERNEL_MULTITASK_H_
#define KERNEL_MULTITASK_H_ 1

#include <stdint.h>

typedef struct Ctx Ctx;

/**
 * @struct Task context struct
 * @details We could add more stuff like parent task and priority
 *
 * @todo Rename to something less generic like Task or TaskCtx, since the fbc
 * also uses fbc_ctx.
 * @todo Use a more standard naming convention for structs. Either all
 * capitalized or all lowercase with "_t" subfix.
 */
struct Ctx {
    Ctx* next;       /**< @brief Pointer to next task */
    Ctx* prev;       /**< @brief Pointer to next task */
    uint32_t stack;  /**< @brief Pointer to the allocated stack for the task */
    uint32_t esp;    /**< @brief Stack top */
    uint32_t cr3;    /**< @brief cr3 register (page directory) */
    uint32_t fxdata; /**< @brief 512 bytes needed for fxsave to store FPU/SSE */
    char* name;      /**< @brief Task name */
};

typedef struct fpu_data_t {
    uint16_t fcw;        /* FPU Control Word. Figure 8-6, vol. 1 */
    uint16_t fsw;        /* FPU Status Word. Figure 8-4, vol. 1 */
    uint8_t ftw;         /* FPU Tag Word */
    uint8_t pad0;        /* Reserved */
    uint16_t fop;        /* FPU Opcode */
    uint32_t fip;        /* FPU Instruction Pointer Offset */
    uint16_t fcs;        /* FPU Instruction Pointer Selector */
    uint16_t pad1;       /* Reserved */
    uint32_t fdp;        /* FPU Instruction Operand Pointer Offset */
    uint16_t fds;        /* FPU Instruction Operand Pointer Selector */
    uint16_t pad2;       /* Reserved */
    uint32_t mxcsr;      /* MXCSR Register State. Figure 10-3, vol. 1 */
    uint32_t mxcsr_mask; /* Mask for MXCSR register */
    uint8_t registers;   /* Registers ST0/MM0 to ST7/MM7 */
} fpu_data_t __attribute__((packed));

/**
 * @struct Tss
 * @brief Task state segment, loaded to the GDT
 */
typedef struct Tss {
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
} Tss __attribute__((packed));

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

/**
 * @brief Allocates and creates a new task with a `name` and `entry` point.
 * @details Defined in src/kernel/multitask.asm
 * @param[in] name The name of the new task.
 * @param[in] entry The entry point of the new task.
 * @return Pointer to the new task's context struct (Ctx).
 */
Ctx* mt_newtask(const char* name, void* entry)
  __attribute__((warn_unused_result));

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
 * @brief Fill the specified fpu_data_t with the current FPU data from fxsave.
 * @details It allocates 512 bytes for fxsave, and it is not optimized. It
 * should only be used for specific debugging. The caller should declare the
 * fpu_data_t. Defined in src/kernel/multitask.asm
 * @param[out] dst Pointer to the allocated fpu_data_t that will be filled.
 */
void mt_get_fpu_data(fpu_data_t* dst);

/**
 * @brief Print the list of tasks starting with the current one.
 * @details Defined in src/kernel/multitask.c
 */
void mt_dump_tasks(void);

/**
 * @brief Print address and members of fpu_data_t struct
 * @param[inout] p Pointer to a fpu_data_t struct
 */
void mt_print_fpu_data(fpu_data_t* p);

#endif /* KERNEL_MULTITASK_H_ */
