
#ifndef _KERNEL_UTIL_H
#define _KERNEL_UTIL_H 1

/**
 * @brief Dump N elements of S size from the stack
 * @details Defined in src/kernel/util.asm
 * @param[in] count The number of elements to print from the stack
 * @param[in] size Size of each element in the stack
 */
void asm_dump_stack(uint32_t count, size_t size);

#endif /* _KERNEL_UTIL_H */
