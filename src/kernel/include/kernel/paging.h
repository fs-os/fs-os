
#ifndef KERNEL_PAGING_H_
#define KERNEL_PAGING_H_ 1

#include <stdint.h>

/**
 * @brief Initialize the page directory and first table, call paging_load() and
 * paging_enable()
 */
void paging_init(void);

/**
 * @brief Display layout of current pages in memory.
 */
void paging_show_map(void);

/**
 * @brief Loads the page directory filled by paging_init() into the CR3 register
 * @details See: src/kernel/paging.asm
 * @param[inout] page_dir The page directory defined in paging.c
 */
void paging_load(uint32_t* page_dir);

/**
 * @brief Enables paging.
 * @details Just sets CR0.PG[bit 31] and CR0.PE[bit 0].
 *
 * Defined in src/kernel/paging.asm
 */
void paging_enable(void);

#endif /* KERNEL_PAGING_H_ */
