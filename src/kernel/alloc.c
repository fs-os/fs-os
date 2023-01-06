
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <kernel/alloc.h>

Block* blk_cursor = (Block*)HEAP_START;

/* init_heap: initializes the heap headers for the allocation functions. */
void init_heap(void) {
    void* first_blk = HEAP_START;

    /* Set the next block ptr to NULL (End of memory) and the block size to the
     * HEAP_SIZE minus the block we just wrote. */
    *(Block*)first_blk = (Block){
        (void*)(first_blk + sizeof(Block)),
        NULL,
        HEAP_SIZE - sizeof(Block),
        1,
    };
}

/* kernel_alloc: allocate "sz" bytes of memory from the heap and return the address
 */
void* kernel_alloc(size_t sz) {
    /* First, make sure the size is aligned to 8 bytes */
    const size_t align_diff = sz % 8;
    if (align_diff != 0)
        sz += 8 - align_diff;

    /* From block cursor (last allocation/free), traverse blocks until we find one
     * free or until we loop back to the original cursor. */
    for (Block* blk = blk_cursor;; blk = blk->next) {
        /* Invalid block, check next */
        if (!blk->free || blk->sz < sz + sizeof(Block)) {
            /* If we are back to where we started, there is no block left. Break */
            if (blk->next == blk_cursor)
                break;
            else
                continue;
        }

        /* Current block ptr + current alloc sz. The pointer is const */
        Block* const new_blk = (Block*)((uint32_t)blk->ptr + sz);

        /* Place the new block header */
        *new_blk = (Block){
            (void*)((uint32_t)new_blk + sizeof(Block)),
            blk->next,                    /* Next block is the old next block */
            blk->sz - sz - sizeof(Block), /* Shrink the allocated size and the new
                                             header isze from the old blk size */
            1,
        };

        blk_cursor = new_blk;

        /* Update values from old block */
        blk->next = new_blk;
        blk->sz   = sz;
        blk->free = 0;

        return blk->ptr;
    }

    /* No block available */
    printf("Error trying to allocate size: 0x%X\n", sz);
    dump_alloc_headers();
    abort("alloc: No block available");
    return NULL;
}

/* kernel_free: free a previously allocated ptr */
void kernel_free(void* ptr) {
    if (!ptr)
        return;

    Block* blk = (Block*)(ptr - sizeof(Block));

    /* If the next block is free, merge */
    if (blk->next->free) {
        /* Add deleted header size and size of old block */
        blk->sz += sizeof(Block) + blk->next->sz;
        blk->next = blk->next->next;
    }

    /* If the next block is being used, just set this one free */
    blk->free = 1;

    /* If blk_cursor was pointing to a smaller block than the one we just freed,
     * update it */
    if (blk->sz > blk_cursor->sz)
        blk_cursor = blk;
}

/* For debugging */

enum header_mod {
    NEW_HEADER = 0,
    MOD_HEADER = 1,
    DEL_HEADER = 2,
    UNK_HEADER = 3,
};

/* print_header: prints information about a single alloc block header */
static inline void print_header(enum header_mod mod, Block* blk) {
    switch (mod) {
        case NEW_HEADER:
            printf("+ ");
            break;
        case MOD_HEADER:
            printf("* ");
            break;
        case DEL_HEADER:
            printf("- ");
            break;
        case UNK_HEADER:
        default:
            break;
    }

    printf("Header: 0x%X | Blk: 0x%X | Next: 0x%X | Sz: 0x%X | Free: %d\n", (uint32_t)blk,
           (uint32_t)blk->ptr, (uint32_t)blk->next, blk->sz, blk->free);
}

/* print_header_id: wrapper for print_header for adding a block id */
static inline void print_header_id(int blk_id, Block* blk) {
    printf("[%d] ", blk_id);
    print_header(UNK_HEADER, blk);
}

/* dump_alloc_headers: prints the information for all the alloc block headers */
void dump_alloc_headers(void) {
    printf("Cursor: 0x%X\n"
           "Dumping heap block headers:\n",
           (uint32_t)blk_cursor);

    /* Block id */
    int i = 0;

    /* From start of the heap, jump to the next block until end of heap. */
    for (Block* blk = HEAP_START; blk != NULL; blk = blk->next) {
        print_header_id(i++, blk);
    }
}

