
#include <stdarg.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* print: calls "putchar" for each char of "str". Returns bytes written. */
static inline int print(const char* str) {
    while (*str != '\0')
        putchar(*str++);

    return 0;
}

/* printi: similar to stdlib's itoan, but instead of writting to buffer, prints. Used
 * by printf's "%i". */
static void printi(int num) {
    /* Write '-' for negative numbers and convert number to positive */
    if (num < 0) {
        putchar('-');
        num = -num;
    }

    /* For more information about this loop, see stdlib's itoan. */
    for (int cur_digit = count_digits(num) - 1; cur_digit >= 0; cur_digit--)
        putchar((num / ipow(10, cur_digit)) % 10 + '0');
}

/* puts: prints "str" and a newline char */
int puts(const char* str) {
    return printf("%s\n", str);
}

/* printf: prints with format "fmt" */
int printf(const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);

    int ret = vprintf(fmt, va);

    va_end(va);
    return ret;
}

/* vprintf: prints with format "fmt" using the variable argument list "va" */
int vprintf(const char* fmt, va_list va) {
    int written = 0;

    while (*fmt != '\0') {
        /* If we get a '%', expect format, if not, just print normal char */
        if (*fmt == '%') {
            fmt++;
            if (written < INT_MAX)
                written++;
            else
                return -1; /* TODO: Set errno to EOVERFLOW */

            /* TODO: Support for more than 1 format char */
            switch (*fmt) {
                case 'c':
                    putchar((char)va_arg(va, int));
                    break;
                case 's':
                    const char* va_str = va_arg(va, const char*);
                    print(va_str);

                    /* If printing a string from va_list, add len to "written" */
                    int va_strlen = strlen(va_str);
                    if (written + va_strlen < INT_MAX)
                        written += va_strlen;
                    else
                        return -1; /* TODO: Set errno to EOVERFLOW */

                    break;
                case 'd':
                    printi(va_arg(va, int));
                    break;
                default:
                    /* If unknown fmt, print the % and the unknown char */
                    putchar('%');
                    putchar(*fmt);
                    break;
            }
        } else {
            /* TODO: Any kind of return value check? */
            putchar(*fmt);
        }

        fmt++;
        if (written < INT_MAX)
            written++;
        else
            return -1; /* TODO: Set errno to EOVERFLOW */
    }

    return written;
}

/* putchar: prints the single character "c" */
int putchar(int c) {
    /* TODO: Implement stdio and write syscalls */
    return c;
}

