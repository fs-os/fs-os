
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <kernel/paging.h>

/* For readability */
#define DIR_ENTRIES   1024
#define TABLE_ENTRIES 1024
#define PAGE_SIZE     4096 /**< @brief In KiB */

/**
 * @def TABLES_MAPPED
 * @details For example 128 tables mapped would map 512MiB of ram: \n
 *   512 MiB / 4 MiB (Memory mapped per table) = 128 filled table entries
 */

#define TABLES_MAPPED DIR_ENTRIES

/**
 * @name Symbols from linker script
 * @{ */
extern uint8_t _text_start;
extern uint8_t _text_end;
extern uint8_t _rodata_start;
extern uint8_t _rodata_end;
extern uint8_t _data_start;
extern uint8_t _data_end;
extern uint8_t _bss_start;
extern uint8_t _bss_end;
/**  @} */

/**
 * @brief Bits for the page directory entries.
 */
enum page_dir_flags {
    PAGEDIR_PRESENT   = 0x1,
    PAGEDIR_READWRITE = 0x2,
    PAGEDIR_USER      = 0x4, /**< @brief If 1, everybody can access it, if 0,
                               only supervisor */
    PAGEDIR_PWT      = 0x8,
    PAGEDIR_PCD      = 0x10,
    PAGEDIR_ACCESSED = 0x20,
    PAGEDIR_AVL0     = 0x40, /**< @brief Unused */
    PAGEDIR_PAGESZ   = 0x80, /**< @brief Pages are 4MiB if this bit is 1 */
    /*  8..11 Available */
    /* 12..31 Bits 12..31 of the address */
};

/**
 * @brief Bits for the page table entries.
 */
enum page_tab_flags {
    PAGETAB_PRESENT   = 0x1,
    PAGETAB_READWRITE = 0x2,
    PAGETAB_USER      = 0x4,
    PAGETAB_PWT       = 0x8,
    PAGETAB_PCD       = 0x10,
    PAGETAB_ACCESSED  = 0x20,
    PAGETAB_DIRTY     = 0x40, /**< @brief It has been written to */
    PAGETAB_PAT       = 0x80, /**< @brief Page attribute table */
    PAGETAB_GLOBAL    = 0x100,
    /*  9..11 Available */
    /* 12..31 Bits 12..31 of the page address */
};

uint32_t page_tables[DIR_ENTRIES][TABLE_ENTRIES] __attribute__((aligned(4096)));
uint32_t page_directory[DIR_ENTRIES] __attribute__((aligned(4096)));

void paging_init(void) {
    /* Initialize empty page directory */
    for (int i = 0; i < DIR_ENTRIES; i++) {
        /*
         * Present bit not set
         * Read and write enabled
         * No user flag (only supervisor)
         * 4KiB in size (not 4MiB)
         */
        page_directory[i] = PAGEDIR_READWRITE;
    }

    /* Initialize the page tables by doing a 1:1 mapping */
    for (uint32_t i = 0; i < TABLES_MAPPED; i++) {
        for (uint32_t j = 0; j < TABLE_ENTRIES; j++) {
            /* For each entry of the table (i), map a new 4096 (PAGE_SIZE) page.
             * We only care about storing bits 12..31 of the address. */
            page_tables[i][j] =
              (i * TABLE_ENTRIES * PAGE_SIZE + j * PAGE_SIZE) |
              PAGETAB_PRESENT | PAGETAB_READWRITE;
        }

        /* Bits 31..12 of the entry are bits 31..12 of the address, no need to
         * shift */
        page_directory[i] =
          ((uint32_t)&page_tables[i][0]) | PAGEDIR_PRESENT | PAGEDIR_READWRITE;
    }

    /* Frame number where the .rodata section starts */
    const uint32_t rodata_start_idx = (uint32_t)&_rodata_start >> 12;

    /* Frame number where the .rodata section ends. Minus 1 because it should be
     * the start of the next section. */
    const uint32_t rodata_end_idx = ((uint32_t)&_rodata_end >> 12) - 1;

    /* Remove write permissions from the page frame where .rodata start to the
     * page frame where it ends (included) */
    for (uint32_t i = rodata_start_idx; i <= rodata_end_idx; i++)
        ((uint32_t*)page_tables)[i] &= ~PAGETAB_READWRITE;

    load_page_dir(page_directory);
    enable_paging();
}

#if 0
/* paging_map: simply map the "vaddr" virtual address to the "paddr" physical address
 * with the specified "flags". Both addresses should be page-aligned. */
void paging_map(void* paddr, void* vaddr, uint16_t flags) {
    /* Last 10 bits, page dir index */
    uint32_t pd_idx = (uint32_t)vaddr >> 22;

    /* Bits 12..20, page table index */
    uint32_t pt_idx = (uint32_t)vaddr >> 12 & 0x03FF;

    /* TODO */
}
#endif

void paging_show_map(void) {
    typedef struct {
        uint32_t dir_i, tab_i;
        uint64_t paddr;
        uint64_t vaddr;
        uint16_t flags;
    } table_entry;

    /* AVL, Global, Page atribute table, Dirty, Accessed, Cache disable,
     * Write-through, User (0 means supervisor), Writable, Present */
    const char* flags_str = "---GPDACWUWP";

    /* Used to compare the current and last address' flags */
    table_entry last_entry = { 0, 0, 0, 0, 0 };

    /* Used to print "..." only once per group of entries with the same flags */
    bool printed_dots = false;

    table_entry e = { 0, 0, 0, 0, 0 };

    /* Iterate page_tables array */
    for (e.dir_i = 0; e.dir_i < DIR_ENTRIES; e.dir_i++) {
        for (e.tab_i = 0; e.tab_i < TABLE_ENTRIES; e.tab_i++) {
            /* Get address stored at current pos and remove flag bits*/
            e.paddr = page_tables[e.dir_i][e.tab_i] & 0xFFFFF000;

            /* Same operation we use for identity mapping (1:1) */
            e.vaddr = e.dir_i * TABLE_ENTRIES * PAGE_SIZE + e.tab_i * PAGE_SIZE;

            /* Get flag bits from current entry */
            e.flags = page_tables[e.dir_i][e.tab_i] & 0x00000FFF;

            /* If we have the same 2 flags in a row, display "..." */
            if (e.flags == last_entry.flags) {
                if (!printed_dots)
                    puts("...");

                last_entry   = e;
                printed_dots = true;

                continue;
            }

            /* If we found a different entry, and we just printed dots, print
             * the last one as well */
            if (printed_dots) {
                printf("[%4ld, %4ld] paddr: 0x%8llX | vaddr: 0x%8llX | flags: ",
                       last_entry.dir_i, last_entry.tab_i, last_entry.paddr,
                       last_entry.vaddr);

                /* Display flags */
                for (int k = 11; k >= 0; k--) {
                    if ((last_entry.flags >> k) & 1)
                        putchar(flags_str[11 - k]);
                    else
                        putchar('-');
                }

                putchar('\n');
            }

            last_entry   = e;
            printed_dots = false;

            printf("[%4ld, %4ld] paddr: 0x%8llX | vaddr: 0x%8llX | flags: ",
                   e.dir_i, e.tab_i, e.paddr, e.vaddr);

            /* Display flags */
            for (int k = 11; k >= 0; k--) {
                if ((e.flags >> k) & 1)
                    putchar(flags_str[12 - k]);
                else
                    putchar('-');
            }

            putchar('\n');
        }
    }
}
