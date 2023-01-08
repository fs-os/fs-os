
#ifndef _STDLIB_H
#define _STDLIB_H

#include <stddef.h>
#include <stdint.h>

/* count_digits: returns the number of digits of a positive num. Will not count "-"
 * for negative numbers */
int count_digits(int64_t num);

/* itoa: write the digits of "num" into "str". "str" needs to have enough space */
void itoa(char* str, int64_t num);

/* ipow: integer power. Returns b^e */
int ipow(int b, int e);

/* itoan: write the first "max_digits" of "num" (at max) into "str". "str" needs to
 * have enough space. Useful for making sure you won't write out of bounds. Keep in
 * mind that max_digits does not include the null terminator. */
void itoan(char* str, int64_t num, size_t max_digits);

/* abort: panic */
void abort(const char* fmt, ...);

/* malloc: allocate "sz" bytes and return a pointer. Memory is not initialized. If
 "sz" is 0, returns NULL. */
void* malloc(size_t sz);

/* calloc: allocate "item_n" items of size "item_sz" and set them to 0 */
void* calloc(size_t item_n, size_t item_sz);

/* free: free a previously allocated ptr */
void free(void* ptr);

#endif /* _STDLIB_H */

