
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h> /* DELME */
#include <kernel/alloc.h>

/* DELME */
static inline void print_header(int mod, Block* blk) {
    switch (mod) {
        case 0:
            printf("+");
            break;
        case 1:
            printf("*");
            break;
        case 2:
            printf("-");
            break;
        default:
            break;
    }
    printf(" Address: %d | Blk: %d | Next: %d | Sz: %d | Free: %d\n", (uint32_t)blk,
           (uint32_t)blk->ptr, (uint32_t)blk->next, blk->sz, blk->free);
}

/* init_heap: initializes the heap headers for the allocation functions. */
void init_heap() {
    void* first_blk = HEAP_START;

    /* Set the next block ptr to NULL (End of memory) and the block size to the
     * HEAP_SIZE minus the block we just wrote. */
    *(Block*)first_blk = (Block){
        (void*)(first_blk + sizeof(Block)),
        NULL,
        HEAP_SIZE - sizeof(Block),
        1,
    };

    /* DELME */
    print_header(1, (Block*)first_blk);
}

/* kernel_alloc: allocate "sz" bytes of memory from the heap and return the address */
void* kernel_alloc(size_t sz) {
    /* From start of the heap, jump to the next block until end of heap. */
    for (Block* blk = HEAP_START; blk != NULL; blk = blk->next) {
        /* Invalid block, check next */
        if (blk->sz < sz || !blk->free)
            continue;

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

        /* Update values from old block */
        blk->next = new_blk;
        blk->sz   = sz;
        blk->free = 0;

        /* DELME */
        print_header(0, blk);
        print_header(1, new_blk);

        return blk->ptr;
    }

    /* No block available */
    abort("alloc: No block available");
    return NULL;
}

/* kernel_free: free a previously allocated ptr */
void kernel_free(void* ptr) {
    if (!ptr)
        return;

    Block* blk = (Block*)(ptr - sizeof(Block));

    blk->free = 1;

    /* If the next block is free, merge */
    if (blk->next->free) {
        /* DELME */
        print_header(2, blk->next);

        /* Add deleted header size and size of old block */
        blk->sz += sizeof(Block) + blk->next->sz;
        blk->next = blk->next->next;
    }

    /* If the next block is being used, just set this one free */

    /* DELME */
    print_header(1, blk);
}

