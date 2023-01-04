
#ifndef _KERNEL_ALLOC_H
#define _KERNEL_ALLOC_H

#include <stdint.h>

#define HEAP_START ((void*)0xA00000) /* Bytes. 10MB */
#define HEAP_SIZE  (0x3200000)       /* Bytes. 50MB */

/* Block header struct. The block ptr should point to (header_ptr + sizeof(Block)) */
typedef struct Block {
    void* ptr;          /* Pointer to block memory. */
    struct Block* next; /* Pointer to next header. End of memory if NULL */
    uint32_t sz;        /* Bytes */
    uint8_t free;       /* 1 or 0 */
} Block;

/* Initialized in src/kernel/alloc.c */
extern Block* blk_cursor;

/* init_heap: initializes the heap headers for the allocation functions. */
void init_heap();

/* kernel_alloc: allocate "sz" bytes of memory from the heap and return the address
 */
void* kernel_alloc(size_t sz);

/* kernel_free: free a previously allocated ptr */
void kernel_free(void* ptr);

/* dump_alloc_headers: prints the information for all the alloc block headers */
void dump_alloc_headers(void);

#endif /* _KERNEL_ALLOC_H */

