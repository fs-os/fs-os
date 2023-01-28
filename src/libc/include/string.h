
#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

/* strlen: return length of "str" without null terminator */
size_t strlen(const char* str) __attribute__((pure)) __attribute__((nonnull));

/* strrev: reverse "str" in place without allocating. Return "str" */
char* strrev(char* str) __attribute__((nonnull));

/* memcmp: compare the first "sz" bytes of 2 pointers. Returns 1 if in the first
 * mismatch, a is greater than b, or -1 if b is greater than a. If all the "sz" bytes
 * in a and b are the same, returns 0. */
int memcmp(const void* a, const void* b, size_t sz) __attribute__((nonnull));

/* memset: sets "sz" bytes of "ptr" to "value". Returns "ptr" */
void* memset(void* ptr, int val, size_t sz) __attribute__((nonnull));

/* memcpy: copies "sz" bytes of "src" into "dst". Returns "dst" */
void* memcpy(void* restrict dst, const void* restrict src, size_t sz)
  __attribute__((nonnull));

#endif /* _STRING_H */

