
#ifndef _KERNEL_PAGING_H
#define _KERNEL_PAGING_H

#include <stdint.h>

/**
 * @brief Initialize the page directory and first table, load the page directory
 * and enable paging
 */
void paging_init(void);

/**
 * @brief Display layout of current pages in memory.
 */
void paging_show_map(void);

/**
 * @brief load the page directory.
 * @details See: src/kernel/paging.asm
 * @param[inout] page_dir The page directory defined in paging.c
 */
void load_page_dir(uint32_t* page_dir);

/**
 * @brief Enable paging.
 * @details Defined in src/kernel/paging.asm
 */
void enable_paging(void);

#endif /* _KERNEL_PAGING_H */
