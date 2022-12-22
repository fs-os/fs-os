
#ifndef _STDLIB_H
#define _STDLIB_H

#include <stddef.h>

/* count_digits: returns the number of digits of a positive num. Will not count "-"
 * for negative numbers */
int count_digits(int num);

/* itoa: write the digits of "num" into "str". "str" needs to have enough space */
void itoa(char* str, int num);

/* ipow: integer power. Returns b^e */
int ipow(int b, int e);


/* itoan: write the first "max_digits" of "num" (at max) into "str". "str" needs to
 * have enough space. Useful for making sure you won't write out of bounds. Keep in
 * mind that max_digits does not include the null terminator. */
void itoan(char* str, int num, size_t max_digits);

#endif
