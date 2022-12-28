
#ifndef _KERNEL_ALLOC_H
#define _KERNEL_ALLOC_H

#include <stdint.h>

#define HEAP_START ((void*)524288) /* Bytes. 512 KB */
#define HEAP_SIZE  (26214400)      /* Bytes. 25MB */

/* Block header struct. The block ptr should point to (header_ptr + sizeof(Block)) */
typedef struct Block {
    void* ptr;          /* Pointer to block memory. */
    struct Block* next; /* Pointer to next header. End of memory if NULL */
    uint32_t sz;        /* Bytes */
    uint8_t free;       /* 1 or 0 */
} Block;

/* init_heap: initializes the heap headers for the allocation functions. */
void init_heap();

/* kernel_alloc: allocate "sz" bytes of memory from the heap and return the address
 */
void* kernel_alloc(size_t sz);

/* kernel_free: free a previously allocated ptr */
void kernel_free(void* ptr);

#endif /* _KERNEL_ALLOC_H */

