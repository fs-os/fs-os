
#ifndef _KERNEL_ALLOC_H
#define _KERNEL_ALLOC_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define HEAP_START ((void*)0xA00000) /* Bytes. 10MB */
#define HEAP_SIZE  (0x3200000)       /* Bytes. 50MB */

typedef struct Block Block;
/**
 * @struct Block
 * @brief Heap block header.
 * @details The block ptr should point to (header_ptr + sizeof(Block))
 */
struct Block {
    Block* prev; /** @brief Pointer to prev header. NULL means start of heap */
    Block* next; /** @brief Pointer to next header. NULL means end of heap */
    uint32_t sz; /** @brief Block size in bytes */
    bool free;   /** @brief True if the block is not being used */
};

/**
 * @var blk_cursor
 * @brief First block that heap_alloc will try to allocate.
 * @details Initialized in src/kernel/alloc.c
 */
extern Block* blk_cursor;

/**
 * @brief Initializes the heap headers for the allocation functions.
 */
void heap_init(void);

/**
 * @brief Allocate `sz` bytes of memory from the heap and return the address
 * @details Returned pointer will be padded/aligned to the parameter.
 * @param[in] sz Size in bytes to allocate
 * @param[in] align Number for aligning the start of the returned pointer
 * @return Pointer to the allocated (aligned) block of memory
 */
void* heap_alloc(size_t sz, size_t align);

/**
 * @brief Free a previously allocated ptr.
 * @details The Block header address will be subtracted from the ptr. Will
 * ignore NULL.
 * @param[out] ptr Pointer to the block of allocated heap memory.
 */
void heap_free(void* ptr);

/**
 * @brief Prints the information for all the alloc block headers.
 */
void heap_dump_headers(void);

#endif /* _KERNEL_ALLOC_H */
