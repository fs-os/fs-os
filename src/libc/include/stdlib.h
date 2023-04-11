
#ifndef _STDLIB_H
#define _STDLIB_H

#include <stddef.h>
#include <stdint.h>

#define RAND_MAX 32768

/**
 * @def panic_line
 * @brief Macro for calling panic() using the current function and line in the
 * file.
 */
#define panic_line(...) panic(__func__, __LINE__, __VA_ARGS__)

/**
 * @brief Returns the number of digits of a positive num.
 * @details Will not count `'-'` for negative numbers.
 * @param[in] num Number for counting.
 * @return Digits of num.
 */
int count_digits(int64_t num) __attribute__((pure));

/**
 * @brief Convert integer to string, write to `str`.
 * @details The `str` parameter is expected to have enough space for `num`.
 * @param[out] str Destination for the converted number.
 * @param[in] num Number to convert.
 */
void itoa(char* str, int64_t num);

/**
 * @brief Convert string to integer.
 * @param[in] str Number in string format.
 * @return Converted number from string.
 */
int atoi(const char* str);

/**
 * @brief Integer power.
 * @param[in] b Base.
 * @param[in] e Exponent.
 * @return `b^e`
 */
int ipow(int b, int e) __attribute__((pure));

/**
 * @brief Convert integer to string with digit limit.
 * @details Will stop when converted `max_digits`. `str` is expected to have
 * enough space, in this case `max_digits + 1` for the NULL terminator.
 * @param[out] str Destination.
 * @param[in] num Number to convert.
 * @param[in] max_digits The maximum number digits to convert.
 */
void itoan(char* str, int64_t num, size_t max_digits);

/**
 * @brief Panics
 * @details Use the panic_line() macro for shorter version
 * @param[in] func The `__func__` macro.
 * @param[in] line The `__LINE__` macro.
 * @param[in] fmt Panic message.
 *
 * @todo Move from stdlib?
 */
void panic(const char* func, unsigned int line, const char* fmt, ...);

/**
 * @brief Panic.
 */
void abort(void);

/**
 * @brief Allocate a block of the specified size in bytes from the heap.
 * @details Memory is not initialized. If `sz` is 0 returns NULL.
 * @param[in] sz Size in bytes to allocate.
 * @return Pointer to the allocated memory block.
 */
void* malloc(size_t sz) __attribute__((warn_unused_result));

/**
 * @brief Allocate the needed bytes for `n` items of size `sz` and initialize
 * them as 0.
 * @param[in] n Number of elements to allocate.
 * @param[in] sz The size of each element.
 * @return Pointer to the allocated memory block.
 */
void* calloc(size_t n, size_t sz) __attribute__((warn_unused_result));

/**
 * @brief Free a previously allocated memory block.
 * @param[out] ptr Pointer to the memory block.
 */
void free(void* ptr);

/**
 * @brief Generate a pseudo-random number from 0 to RAND_MAX.
 * @return Pseudo-random integer.
 */
int rand(void);

/**
 * @brief Set the rand seed.
 * @param[in] seed New rand seed.
 */
void srand(unsigned int seed);

#endif /* _STDLIB_H */
