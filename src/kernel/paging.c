
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <kernel/paging.h>

#define DIR_ENTRIES   1024 /* Array size */
#define TABLE_ENTRIES 1024 /* Array size*/
#define PAGE_SIZE     4096 /* KiB */

/**
 * @def TABLES_MAPPED
 * @details For example 128 tables mapped would map 512MiB of ram:
 *   512 MiB / 4 MiB (Memory mapped per table) = 128 filled table entries
 */
#define TABLES_MAPPED DIR_ENTRIES

/**
 * @enum page_dir_flags
 * @brief Bits for the page directory entries.
 */
enum page_dir_flags {
    PAGEDIR_PRESENT   = 0x01,
    PAGEDIR_READWRITE = 0x02,
    PAGEDIR_USER      = 0x04, /* If clear, only supervisor can access page */
    PAGEDIR_PWT       = 0x08,
    PAGEDIR_PCD       = 0x10,
    PAGEDIR_ACCESSED  = 0x20,
    PAGEDIR_AVL0      = 0x40, /* Unused */
    PAGEDIR_PAGESZ    = 0x80, /* If set, pages are 4MiB. Otherwise, 4KiB */
    /* Bits 09..11 of entry are available */
    /* Bits 12..31 of entry are bits 12..31 of the page frame address */
};

/**
 * @enum page_tab_flags
 * @brief Bits for the page table entries.
 */
enum page_tab_flags {
    PAGETAB_PRESENT   = 0x001,
    PAGETAB_READWRITE = 0x002,
    PAGETAB_USER      = 0x004,
    PAGETAB_PWT       = 0x008,
    PAGETAB_PCD       = 0x010,
    PAGETAB_ACCESSED  = 0x020,
    PAGETAB_DIRTY     = 0x040, /* It has been written to */
    PAGETAB_PAT       = 0x080, /* Page attribute table */
    PAGETAB_GLOBAL    = 0x100,
    /* Bits 09..11 of entry are available */
    /* Bits 12..31 of entry are bits 12..31 of the page address */
};

/* Symbols from linker script */
extern uint8_t _text_start;
extern uint8_t _text_end;
extern uint8_t _rodata_start;
extern uint8_t _rodata_end;
extern uint8_t _data_start;
extern uint8_t _data_end;
extern uint8_t _bss_start;
extern uint8_t _bss_end;

/* Page directory and page tables, filled and loaded in paging_init() */
static uint32_t page_directory[DIR_ENTRIES] __attribute__((aligned(4096)));
static uint32_t page_tables[DIR_ENTRIES][TABLE_ENTRIES]
  __attribute__((aligned(4096)));

void paging_init(void) {
    /* Initialize empty page directory, will be overwritten next for mapped
     * entries (in our case 1:1 mapping of entire memory) */
    for (int i = 0; i < DIR_ENTRIES; i++)
        page_directory[i] = 0;

    /* Initialize the page tables by doing a 1:1 mapping */
    for (uint32_t i = 0; i < TABLES_MAPPED; i++) {
        for (uint32_t j = 0; j < TABLE_ENTRIES; j++) {
            /* For each entry of the table (i), map a new 4096 (PAGE_SIZE) page.
             * We only care about storing bits 12..31 of the address. */
            page_tables[i][j] = (i * TABLE_ENTRIES + j) * PAGE_SIZE;
            page_tables[i][j] |= PAGETAB_PRESENT | PAGETAB_READWRITE;
        }

        /* Bits 12..31 of the entry are bits 12..31 of the address, no need to
         * shift */
        page_directory[i] = (uint32_t)&page_tables[i][0];
        page_directory[i] |= PAGEDIR_PRESENT | PAGEDIR_READWRITE;
    }

    /* Frame number where the .rodata section starts */
    const uint32_t rodata_start_idx = (uint32_t)&_rodata_start >> 12;

    /* Frame number where the .rodata section ends. Minus 1 because it should be
     * the start of the next section. */
    const uint32_t rodata_end_idx = ((uint32_t)&_rodata_end >> 12) - 1;

    /* Convert to 1D array */
    uint32_t* page_frames = (uint32_t*)page_tables;

    /* Remove write permissions from the page frame where .rodata start to the
     * page frame where it ends (included) */
    for (uint32_t i = rodata_start_idx; i <= rodata_end_idx; i++)
        page_frames[i] &= ~PAGETAB_READWRITE;

    paging_load(page_directory);
    paging_enable();
}

void paging_show_map(void) {
    typedef struct {
        uint32_t dir_i, tab_i;
        uint64_t paddr;
        uint64_t vaddr;
        uint16_t flags;
    } table_entry;

    /* AVL, Global, Page atribute table, Dirty, Accessed, Cache disable,
     * Write-through, User (0 means supervisor), Writable, Present */
    static const char* flags_str = "---GPDACWUWP";

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
                printf("[%4ld, %4ld] paddr: 0x%08llX | vaddr: 0x%08llX | "
                       "flags: ",
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

            printf("[%4ld, %4ld] paddr: 0x%08llX | vaddr: 0x%08llX | flags: ",
                   e.dir_i, e.tab_i, e.paddr, e.vaddr);

            /* Display flags */
            for (int k = 11; k >= 0; k--) {
                if ((e.flags >> k) & 1)
                    putchar(flags_str[11 - k]);
                else
                    putchar('-');
            }

            putchar('\n');
        }
    }
}
