
#ifndef STRING_H_
#define STRING_H_ 1

#include <stddef.h>

/**
 * @brief Get the length of the specified string.
 * @details Does not include the NULL terminator.
 * @param[in] str String for getting the length.
 * @return Length of `str`.
 */
size_t strlen(const char* str) __attribute__((pure)) __attribute__((nonnull));

/**
 * @brief Reverse string in place.
 * @details Doesnt allocate memory.
 * @param[out] str String to reverse in place.
 * @return Returns the same string pointer passed as argument.
 */
char* strrev(char* str) __attribute__((nonnull));

/**
 * @brief Compare the first `sz` bytes of 2 memory locations.
 * @param[in] a First pointer to compare.
 * @param[in] b Second pointer to compare.
 * @param[in] sz Ammount of bytes to check.
 * @return
 *  - **1** if in the first mismatch, `a` is greater than `b`.
 *  - **-1** if in the first mismatch, `b` is greater than `a`.
 *  - **0** if the first `sz` bytes are the same.
 */
int memcmp(const void* a, const void* b, size_t sz) __attribute__((nonnull));

/**
 * @brief Set `sz` bytes of `ptr` to `value`.
 * @param[out] ptr Memory location to change.
 * @param[in] val New value for the bytes.
 * @param[in] sz Number of bytes to overwrite.
 * @return The `ptr` argument.
 */
void* memset(void* ptr, int val, size_t sz) __attribute__((nonnull));

/**
 * @brief Copy `sz` bytes of `src` into `dst`.
 * @param[out] dst Destination address.
 * @param[in] src Source address.
 * @param[in] sz Number of bytes to copy from `src` to `dst`.
 * @return The `dst` argument.
 */
void* memcpy(void* restrict dst, const void* restrict src, size_t sz)
  __attribute__((nonnull));

/**
 * @brief Compare 2 strings.
 * @param[in] a First string to compare.
 * @param[in] b Second string to compare.
 * @return
 *  - **Less or equal than 1** if in the first mismatch, a is greater than b.
 *  - **Less or equal than -1** if in the first mismatch, b is greater than a.
 *  - **0** if they are equal.
 */
int strcmp(const char* a, const char* b);

#endif /* STRING_H_ */
