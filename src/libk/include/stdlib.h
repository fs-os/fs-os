
#ifndef STDLIB_H_
#define STDLIB_H_ 1

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
 * @brief Returns the number of digits of a positive integer.
 * @details Will not count `'-'` for negative numbers.
 * @param[in] num Number for counting.
 * @return Digits of num.
 */
int digits_int(int64_t num) __attribute__((pure));

/**
 * @brief Returns the hexadecimal digits of unsigned integer `num`.
 * @details Example: 123 -> 0x7B -> 2 hex digits
 * @param[in] num Number for counting.
 * @return Digits of hexadecimal version of num.
 */
int digits_hex(uint64_t num) __attribute__((pure));

/**
 * @brief Returns the number of digits of a positive double.
 * @details Will not count `'-'` for negative numbers. Will count the dot.
 * @param[in] num Decimal number for counting.
 * @param[in] decilams Max number of decimal places.
 * @return Digits of num.
 */
int digits_double(double num, uint32_t decimals) __attribute__((pure));

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
 * @brief Kernel panic. Print message and halt.
 * @details Use the panic_line() macro for shorter version
 * @param[in] func The name of the function. Should use the `__func__` macro. If
 * NULL, panic() won't print function and line.
 * @param[in] line The line of the panic, should use the `__LINE__` macro.
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
void srand(uint32_t seed);

#endif /* STDLIB_H_ */
