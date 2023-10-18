
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* memset */
#include <kernel/heap.h>

/**
 * @brief Returns the pointer to the actual usable memory of a Block
 */
#define HEADER_TO_PTR(blk) ((void*)((uint32_t)(blk) + sizeof(Block)))

static Block* const first_block = (Block*)HEAP_START;

void heap_init(void) {
    memset(HEAP_START, 0, HEAP_SIZE);

    *first_block = (Block){
        .next = NULL,                      /* It's the last block */
        .prev = NULL,                      /* And first block */
        .sz   = HEAP_SIZE - sizeof(Block), /* Size of heap - this block */
        .free = true,                      /* Start free */
    };
}

void* heap_alloc(size_t sz, size_t align) {
    /* From first block, traverse linked list until we find a free one or until
     * we reach the end. */
    for (Block* blk = first_block;; blk = blk->next) {
        /* Is it an invalid block? */
        if (!blk->free || blk->sz < sz + sizeof(Block)) {
            /* If we reached the end of the linked list, exit loop and panic.
             * Otherwise, continue to the next block */
            if (blk->next == NULL)
                break;
            else
                continue;
        }

        /* Add padding to the end of the last block so the data of the new block
         * is aligned. */
        size_t sz_pad = (uint32_t)HEADER_TO_PTR(blk) % align;
        if (sz_pad != 0) {
            /* Once we know we need padding, update var to know how much */
            sz_pad = align - sz_pad;

            /* Check if the updated size is still enough to hold what was
             * requested */
            if (blk->sz - sz_pad < sz)
                continue;

            /* If the start of the data is not algined, move current 'blk' */
            Block tmp = *blk;
            memset(blk, 0, sizeof(Block));
            blk  = (Block*)((uint32_t)blk + sz_pad);
            *blk = tmp;

            /* Update current 'blk->sz' since we moved the header */
            blk->sz -= sz_pad;

            /* If this is not the first block, also update the size and new
             * location in the previous item */
            if (blk->prev != NULL) {
                blk->prev->sz += sz_pad;
                blk->prev->next = blk;
            }

            /* Same if there is a next one */
            if (blk->next != NULL)
                blk->next->prev = blk;
        }

        /* Location of the new block we will add after the size we are
         * allocating: (data ptr + sz we just allocated)
         * The pointer is const. */
        Block* const new_blk = (Block*)((uint32_t)HEADER_TO_PTR(blk) + sz);

        /* Place the new block header */
        *new_blk = (Block){
            .next = blk->next, /* The ".next" of the old block */
            .prev = blk,       /* The old block */
            .sz   = blk->sz - sz - sizeof(Block), /* Shrink the old blk sz */
            .free = true,
        };

        /* Update values from old block */
        blk->next = new_blk;
        blk->sz   = sz;
        blk->free = false;

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

        /* Once we are done merging with prev, we update the blk ptr */
        blk = blk->prev;
    }

    /* If the next block is being used, just set this one free */
    blk->free = true;
}

void* heap_calloc(size_t item_n, size_t item_sz, size_t align) {
    const size_t bytes = item_n * item_sz;
    void* ptr          = heap_alloc(bytes, align);

    switch (item_sz) {
        case 2: { /* sizeof(uint16_t) */
            uint16_t* casted = ptr;
            for (size_t i = 0; i < item_n; i++)
                casted[i] = 0;
            break;
        }
        case 4: { /* sizeof(uint32_t) */
            uint32_t* casted = ptr;
            for (size_t i = 0; i < item_n; i++)
                casted[i] = 0;
            break;
        }
        case 1: /* sizeof(uint8_t) || item_sz > sizeof(uint32_t) */
        default: {
            /* If it's an unknown size, iterate each byte */
            uint8_t* casted = ptr;
            for (size_t i = 0; i < bytes; i++)
                casted[i] = 0;
            break;
        }
    }

    return ptr;
}

/*----------------------------------------------------------------------------*/

enum header_mod {
    NEW_HEADER = 0,
    MOD_HEADER = 1,
    DEL_HEADER = 2,
    UNK_HEADER = 3,
};

void heap_dump_headers(void) {
    int i = 0;

    /* From start of the heap, jump to the next block until end of heap. */
    for (Block* blk = HEAP_START; blk != NULL; blk = blk->next, i++) {
        printf("[%d] [%c] Header: %p | Data: %p | Next: %p", i,
               (blk->free) ? 'F' : 'B', blk, HEADER_TO_PTR(blk), blk->next);

        /* Paddings for "(null)" */
        if (blk->next == NULL)
            printf("  ");

        printf(" | Prev: %p", blk->prev);

        if (blk->prev == NULL)
            printf("  ");

        printf(" | Sz: 0x%07lX\n", blk->sz);
    }
}
