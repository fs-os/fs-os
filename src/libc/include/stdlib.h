
#ifndef _STDLIB_H
#define _STDLIB_H

#include <stddef.h>
#include <stdint.h>

#define panic_line(...) panic(__func__, __LINE__, __VA_ARGS__)

/* count_digits: returns the number of digits of a positive num. Will not count "-"
 * for negative numbers */
int count_digits(int64_t num) __attribute__((pure));

/* itoa: write the digits of "num" into "str". "str" needs to have enough space */
void itoa(char* str, int64_t num);

/* atoi: convert the digits of "str" to an integer, and return it */
int atoi(const char* str);

/* ipow: integer power. Returns b^e */
int ipow(int b, int e) __attribute__((pure));

/* itoan: write the first "max_digits" of "num" (at max) into "str". "str" needs to
 * have enough space. Useful for making sure you won't write out of bounds. Keep in
 * mind that max_digits does not include the null terminator. */
void itoan(char* str, int64_t num, size_t max_digits);

/* panic: "func" should be the __func__ macro and "line" should be the
 * __LINE__ macro. Use the "panic_line" macro for shorter version */
/* TODO: Move from stdlib? */
void panic(const char* func, unsigned int line, const char* fmt, ...);

/* abort: panic */
void abort(void);

/* malloc: allocate "sz" bytes and return a pointer. Memory is not initialized. If
 "sz" is 0, returns NULL. */
void* malloc(size_t sz) __attribute__((warn_unused_result));

/* calloc: allocate "item_n" items of size "item_sz" and set them to 0 */
void* calloc(size_t item_n, size_t item_sz) __attribute__((warn_unused_result));

/* free: free a previously allocated ptr */
void free(void* ptr);

#endif /* _STDLIB_H */

