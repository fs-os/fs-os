
#ifndef KERNEL_HEAP_H_
#define KERNEL_HEAP_H_ 1

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
 *
 * @todo Rename to something like HeapBlk
 */
struct Block {
    Block* next; /** @brief Pointer to next header. NULL means end of heap */
    Block* prev; /** @brief Pointer to prev header. NULL means start of heap */
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
 * @details Returned pointer will be padded/aligned to the `align` parameter.
 * @param[in] sz Size in bytes to allocate
 * @param[in] align Alignment for the returned pointer
 * @return Pointer to the allocated block of memory
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
 * @brief Allocate `item_n` items of size `item_sz`, set them to zero, and
 * return the pointer
 * @details Returned pointer will be padded/aligned to the `align` parameter.
 * @param[in] item_n Number of items to allocate
 * @param[in] item_sz The size of each item
 * @param[in] align Alignment for the returned pointer
 * @return Pointer to the allocated and initialized block of memory
 */
void* heap_calloc(size_t item_n, size_t item_sz, size_t align);

/**
 * @brief Prints the information for all the alloc block headers.
 */
void heap_dump_headers(void);

#endif /* KERNEL_HEAP_H_ */
