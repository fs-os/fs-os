
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/heap.h>
#include <kernel/framebuffer_console.h> /* Color to panic() */
#include <kernel/color.h>               /* Color to panit() */

/* count_digits: returns the number of digits of a positive num. Will not count "-"
 * for negative numbers */
int count_digits(int64_t num) {
    int ret = 1;

    if (num < 0)
        num = -num;

    /* Count how many numbers we can remove */
    while ((num /= 10) > 0) {
        ret++;
    }

    return ret;
}

/* itoa: write the digits of "num" into "str". "str" needs to have enough space */
void itoa(char* str, int64_t num) {
    int sign   = 0;
    int digits = count_digits(num);

    if (num < 0) {
        sign   = 1;
        num    = -num;
        str[0] = '-';
    }

    /* Since we know the last idx, write null terminator */
    str[digits + sign] = '\0';

    /*
     * Add 1 if num is negative (for "-")
     * Subtract 1 because we start on the string's idx (3 digits -> 2 last idx)
     *
     * Loop until i < sign (instead of until 0), because if sign is 1 (num is
     * negative), we want to stop before writing "-" (last pos is 1).
     */
    for (int i = digits + sign - 1; i >= sign; i--) {
        str[i] = num % 10 + '0';
        num /= 10;
    }
}

/* atoi: convert the digits of "str" to an integer, and return it */
int atoi(const char* str) {
    int ret = 0;

    /* Go to the first digit of the string */
    while (*str != '-' && (*str < '0' || *str > '9'))
        str++;

    int sign = 1;
    if (*str == '-') {
        sign = -1;
        str++;
    }

    /* We assume that if we encounter a '-', the number is inmediately after */
    while (*str >= '0' && *str <= '9') {
        ret *= 10;
        ret += *str - '0';
        str++;
    }

    /* Make negative if needed and return */
    return ret * sign;
}

/* ipow: integer power. Returns b^e */
int ipow(int b, int e) {
    int ret = 1;

    /* Multiply by the base "e" times */
    while (e-- > 0)
        ret *= b;

    return ret;
}

/* itoan: write the first "max_digits" of "num" (at max) into "str". "str" needs to
 * have enough space. Useful for making sure you won't write out of bounds. Keep in
 * mind that max_digits does not include the null terminator. */
void itoan(char* str, int64_t num, size_t max_digits) {
    if (max_digits <= 0) {
        str[0] = '\0';
        return;
    }

    /* sp is the current position we want to write in the string */
    size_t sp = 0;

    /* Write '-' for negative numbers and convert number to positive */
    if (num < 0) {
        str[sp++] = '-';
        num       = -num;
    }

    /*
     * cur_digit will count the current number we want to write, starting from the
     * right, where the last one is 0. So for 354, cur_digit will be 2, and 354 / 100
     * is 3. If cur_digit is 1, 354 / 10 = 35, and we can extract the last digit by
     * doing (n%10)
     *
     * Also make sure the string position is not greater than the max we can write.
     */
    int cur_digit;
    for (cur_digit = count_digits(num) - 1; cur_digit >= 0 && sp < max_digits;
         cur_digit--)
        str[sp++] = (num / ipow(10, cur_digit)) % 10 + '0';

    str[sp++] = '\0';
}

/* panic: "func" should be the __func__ macro and "line" should be the
 * __LINE__ macro. Use the "panic_line" macro for shorter version */
/* TODO: Move from stdlib? */
void panic(const char* func, unsigned int line, const char* fmt, ...) {
    /* TODO: Proper kernel panic */

    if (func == NULL)
        func = "???";

    va_list va;
    va_start(va, fmt);

    fbc_setfore(COLOR_RED_B);
    printf("[%s:%d] kernel panic: ", func, line);
    fbc_setfore(COLOR_RED);
    vprintf(fmt, va);

    va_end(va);

    asm volatile("hlt");

    for (;;)
        ;

    __builtin_unreachable();
}

/* abort: panic */
void abort(void) {
    puts("kernel panic: abort");

    asm volatile("hlt");

    for (;;)
        ;

    __builtin_unreachable();
}

/* malloc: allocate "sz" bytes and return a pointer. Memory is not initialized. If
 "sz" is 0, returns NULL. */
void* malloc(size_t sz) {
    return heap_alloc(sz);
}

/* calloc: allocate "item_n" items of size "item_sz" and set them to 0 */
void* calloc(size_t item_n, size_t item_sz) {
    const size_t bytes = item_n * item_sz;

    void* ptr = malloc(bytes);
    for (size_t i = 0; i < bytes; i++) {
        ((uint8_t*)ptr)[i] = 0;
    }

    return ptr;
}

/* free: free a previously allocated ptr */
void free(void* ptr) {
    heap_free(ptr);
}

/* rand_next: used by the rand and srand functions */
static uint32_t rand_next = 1;

/* rand: returns a pseudo-random number from 0 to RAND_MAX */
int rand(void) {
    rand_next = (rand_next * 1103515245) + 12345;
    return (unsigned int) (rand_next / 65536) % RAND_MAX;
}

/* srand: sets the seed for the rand function */
void srand(unsigned int seed) {
    rand_next = seed;
}

