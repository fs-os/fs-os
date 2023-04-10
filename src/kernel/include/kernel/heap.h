
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
 *
 * The block ptr should point to (header_ptr + sizeof(Block))
 */
struct Block {
    Block* prev; /** @brief Pointer to prev header. Start of heap means NULL */
    Block* next; /** @brief Pointer to next header. End of heap means NULL */
    uint32_t sz; /** @brief Block size in bytes */
    bool free;   /** @brief True if the block is not being used */
};

/**
 * @var blk_cursor
 * @brief First block that heap_alloc will try to allocate.
 *
 * Initialized in src/kernel/alloc.c
 */
extern Block* blk_cursor;

/**
 * @brief Initializes the heap headers for the allocation functions.
 */
void heap_init(void);

/**
 * @brief Allocate \p sz bytes of memory from the heap and return the address
 * @details Size will be padded to 8 bytes.
 * @param[in] sz Size in bytes to allocate
 * @return Pointer to the allocated block of memory
 */
void* heap_alloc(size_t sz);

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
