
#ifndef _KERNEL_PAGING_H
#define _KERNEL_PAGING_H

#include <stdint.h>

/* paging_init: initialize the page directory and first table, load the page
 * directory and enable paging */
void paging_init(void);

/* paging_show_map: display layout of current pages in memory */
void paging_show_map(void);

/* load_page_dir: load the page directory. See:
 * src/kernel/paging_asm.asm */
void load_page_dir(uint32_t* page_dir);

/* enable_paging: defined in src/kernel/paging_asm.asm */
void enable_paging(void);

#endif /* _KERNEL_PAGING_H */
