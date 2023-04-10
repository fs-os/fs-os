
#ifndef _KERNEL_ALLOC_H
#define _KERNEL_ALLOC_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define HEAP_START ((void*)0xA00000) /* Bytes. 10MB */
#define HEAP_SIZE  (0x3200000)       /* Bytes. 50MB */

/** @todo Continue with doxygen format */

/* Block header struct. The block ptr should point to (header_ptr +
 * sizeof(Block)) */
typedef struct Block Block;
struct Block {
    Block* prev; /* Pointer to prev header. Begining of heap if NULL */
    Block* next; /* Pointer to next header. End of heap if NULL */
    uint32_t sz; /* Bytes */
    bool free;   /* 1 or 0 */
};

/* Initialized in src/kernel/alloc.c */
extern Block* blk_cursor;

/* init_heap: initializes the heap headers for the allocation functions. */
void heap_init(void);

/* heap_alloc: allocate "sz" bytes of memory from the heap and return the
 * address */
void* heap_alloc(size_t sz);

/* heap_free: free a previously allocated ptr */
void heap_free(void* ptr);

/* heap_dump_headers: prints the information for all the alloc block headers */
void heap_dump_headers(void);

#endif /* _KERNEL_ALLOC_H */

