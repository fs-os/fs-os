
#include <stdint.h>
#include <kernel/paging.h>

enum page_dir_flags {
    PAGEDIR_PRESENT   = 0x1, /* 00000001 */
    PAGEDIR_READWRITE = 0x2, /* 00000010 */
    PAGEDIR_USER      = 0x4, /* 00000100. If 1, everybody can access it, if 0, only
                                supervisor */
    PAGEDIR_PWT      = 0x8,  /* 00001000 */
    PAGEDIR_PCD      = 0x10, /* 00010000 */
    PAGEDIR_ACCESSED = 0x20, /* 00100000 */
    PAGEDIR_AVL0     = 0x40, /* 01000000. Unused */
    PAGEDIR_PAGESZ   = 0x80, /* 10000000. Pages are 4MiB if this bit is 1 */
    /*  8..11 Available */
    /* 12..31 Bits 12..31 of the address */
};

enum page_tab_flags {
    PAGETAB_PRESENT   = 0x1,   /* 0000 0000 0001 */
    PAGETAB_READWRITE = 0x2,   /* 0000 0000 0010 */
    PAGETAB_USER      = 0x4,   /* 0000 0000 0100 */
    PAGETAB_PWT       = 0x8,   /* 0000 0000 1000 */
    PAGETAB_PCD       = 0x10,  /* 0000 0001 0000 */
    PAGETAB_ACCESSED  = 0x20,  /* 0000 0010 0000 */
    PAGETAB_DIRTY     = 0x40,  /* 0000 0100 0000. It has been written to */
    PAGETAB_PAT       = 0x80,  /* 0000 1000 0000. Page attribute table */
    PAGETAB_GLOBAL    = 0x100, /* 0001 0000 0000 */
    /*  9..11 Available */
    /* 12..31 Bits 12..31 of the page address */
};

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));

/* paging_init: initialize the page directory and first table, load the page
 * directory and enable paging */
void paging_init(void) {
    /* Initialize empty page directory */
    for (int i = 0; i < 1024; i++) {
        /*
         * Present bit not set
         * Read and write enabled
         * No user flag (only supervisor)
         * 4KiB in size (not 4MiB)
         */
        page_directory[i] = PAGEDIR_READWRITE;
    }

    /* Initialize empty page table */
    for (int i = 0; i < 1024; i++) {
        /* Increase the bit 12 of the table entry each iteration (lowest bit of the
         * address) */
        first_page_table[i] = (i * 0x1000) | PAGETAB_PRESENT | PAGETAB_READWRITE;
    }

    /* Bits 31..12 of the entry are bits 31..12 of the address, no need to shift */
    page_directory[0] =
      ((uint32_t)&first_page_table[0]) | PAGEDIR_PRESENT | PAGEDIR_READWRITE;

    load_page_dir(page_directory);
    enable_paging();
}

