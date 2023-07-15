
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* memset */
#include <kernel/heap.h>

/**
 * @brief Returns the pointer to the actual usable memory of a Block
 */
#define HEADER_TO_PTR(blk) ((void*)((uint32_t)blk + sizeof(Block)))

Block* blk_cursor = (Block*)HEAP_START;

void heap_init(void) {
    void* first_blk = HEAP_START;

    *(Block*)first_blk = (Block){
        NULL,                      /* First block */
        NULL,                      /* And last block */
        HEAP_SIZE - sizeof(Block), /* Size of block is size of heap - this
                                    * block */
        1,                         /* Start free */
    };
}

void* heap_alloc(size_t sz, size_t align) {
    /* From block cursor (last allocation/free), traverse blocks until we find
     * one free or until we loop back to the original cursor. */
    for (Block* blk = blk_cursor;; blk = blk->next) {
        /* Invalid block, check next */
        if (!blk->free || blk->sz < sz + sizeof(Block)) {
            /**
             * @todo Pretty sure this isn't supposed to work since last block
             * is NULL, doesn't "loop". We need to find a new way to loop, not
             * chaging the NULL part since we need to know which block is the
             * first and last
             */

            /* If we are back to where we started, there is no block left */
            if (blk->next == blk_cursor)
                break;
            else
                continue;
        }

        /* Add padding to the end of the last block so the data of the new block
         * is aligned. */
        size_t sz_pad = (uint32_t)HEADER_TO_PTR(blk) % align;
        if (sz_pad != 0 && blk->prev != NULL) {
            /* Once we know we need padding, update var to know how much */
            sz_pad = align - sz_pad;

            /* If the start of the data is not algined and this is not the first
             * block, add padding to size of last block */
            blk->prev->sz += sz_pad;

            /* Move current 'blk' since we updated the prev's sz */
            Block tmp = *blk;
            memset(blk, 0, sizeof(Block));
            blk  = (Block*)((uint32_t)blk + sz_pad);
            *blk = tmp;

            /* Update current 'blk->sz' since we moved the header */
            blk->sz -= sz_pad;
        }

        /* Location of the new block we will add after the size we are
         * allocating: (data ptr + sz we just allocated)
         * The pointer is const. */
        Block* const new_blk = (Block*)((uint32_t)HEADER_TO_PTR(blk) + sz);

        /* Place the new block header */
        *new_blk = (Block){
            blk,       /* Prev block is the old block */
            blk->next, /* Next block is ".next" of the old block */
            blk->sz - sz - sizeof(Block), /* Shrink the old blk sz */
            1,
        };

        blk_cursor = new_blk;

        /* Update values from old block */
        blk->next = new_blk;
        blk->sz   = sz;
        blk->free = 0;

        /* Return the pointer to the actual usable memory:
         * (blk + sizeof(Block)) */
        return HEADER_TO_PTR(blk);
    }

    /* No block available */
    printf("Error trying to allocate size: 0x%lX\n", sz);
    heap_dump_headers();
    panic_line("alloc: No block available");
    return NULL;
}

void heap_free(void* ptr) {
    if (!ptr)
        return;

    Block* blk = (Block*)(ptr - sizeof(Block));

    /* If this is not the last block, and the next block is free, merge */
    if (blk->next != NULL && blk->next->free) {
        /* Add deleted header size and size of old block */
        blk->sz += sizeof(Block) + blk->next->sz;

        /*
         * If the next block isn't the last one, make the "next" of that one
         * point to us. Then make the current "next" point to the next's "next",
         * even if null:
         *
         *  [blk] -> [blk->next] -> [blk->next->next]
         *   | ^                         | ^
         *   | |----------(prev)---------| |
         *   |------------(next)-----------|
         */
        if (blk->next->next != NULL)
            blk->next->next->prev = blk;

        blk->next = blk->next->next;
    }

    /* If this is not the first block, and the prev block is free, merge */
    if (blk->prev != NULL && blk->prev->free) {
        /* Add deleted header size and size of old block */
        blk->prev->sz += sizeof(Block) + blk->sz;

        /*
         * If the current block isn't the last one, make the previous block's
         * "next" point to that. Then make the "next" pointer of the "prev"
         * block point to the current block's "next", even if null:
         *
         *  [blk->prev] -> [blk] -> [blk->next]
         *      | ^                     | ^
         *      | |--------(prev)-------| |
         *      |----------(next)---------|
         */
        if (blk->next != NULL)
            blk->next->prev = blk->prev;

        blk->prev->next = blk->next;
    }

    /* If the next block is being used, just set this one free */
    blk->free = 1;

    /* If blk_cursor was pointing to a smaller block than the one we just freed,
     * update it */
    if (blk->sz > blk_cursor->sz)
        blk_cursor = blk;
}

enum header_mod {
    NEW_HEADER = 0,
    MOD_HEADER = 1,
    DEL_HEADER = 2,
    UNK_HEADER = 3,
};

/**
 * @brief Prints information about a single alloc block header
 * @param blk_id The current block number
 * @param blk Pointer to the Block structure we want to print
 */
static inline void print_header_id(int blk_id, Block* blk) {
    printf("[%d] ", blk_id);
    printf("Header: %p | Blk: %p | Prev: %p | Next: %p | Sz: 0x%lX | Free: "
           "%d\n",
           blk, HEADER_TO_PTR(blk), blk->prev, blk->next, blk->sz, blk->free);
}

void heap_dump_headers(void) {
    printf("Cursor: %p\n"
           "Dumping heap block headers:\n",
           blk_cursor);

    /* Block id */
    int i = 0;

    /* From start of the heap, jump to the next block until end of heap. */
    for (Block* blk = HEAP_START; blk != NULL; blk = blk->next) {
        print_header_id(i++, blk);
    }
}
