
#ifndef KERNEL_UTIL_H_
#define KERNEL_UTIL_H_ 1

/**
 * @brief Dump N elements of S size from the stack
 * @details Defined in src/kernel/util.asm
 * @param[in] count The number of elements to print from the stack
 * @param[in] size Size of each element in the stack
 */
void asm_dump_stack(uint32_t count, size_t size);

/**
 * @brief Enables hardware debugging exceptions
 * @details Enables the Trap Flag (TF) bit in EFLAGS. If this bit is true, the
 * CPU will throw a Debug (id 1) exception each instruction. If `on_branch`,
 * also enables the LBR and BTF bits in the MSR registers. If the BTF bit is
 * true, it will throw an exception each branch switch instead of each
 * instruction. See function definition for more details.
 *
 * Defined in src/kernel/util.asm
 * @param[in] on_branch If true, it will throw debug exception on branch switch
 * instead of on instruction.
 *
 * @todo This debug method is not really useful since by enabling debug
 * exceptions on branch switch, it also means that it will throw an exception
 * when returning from the ISR. You can disable them inside the ISR, but you
 * have to re-enable them before you return, and that causes another exception.
 */
void asm_enable_debug(uint32_t on_branch);

/**
 * @brief Disables hardware debugging exceptions
 * @details Disabled the Trap Flag (TF) bit in EFLAGS. If `on_branch`,
 * also disabled the LBR and BTF bits in the MSR registers. See function
 * definition for more details.
 *
 * Defined in src/kernel/util.asm
 * @param[in] on_branch If true, it will also disable LBR and BTF flags.
 */
void asm_disable_debug(uint32_t on_branch);

#endif /* KERNEL_UTIL_H_ */
