
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * @brief Prints the speicified string using putchar.
 * @param[in] str Zero terminated string to print.
 * @return Bytes written.
 */
static inline int print(const char* str) {
    while (*str != '\0')
        putchar(*str++);

    return 0;
}

/**
 * @brief Print string with padding.
 * @details Similar to print(), but adds `strlen(str) - pad` spaces before
 * "str". Used for "%123s"
 * @param[in] str String to print.
 * @param[in] pad Padding for the string.
 */
static void prints_n(const char* str, uint32_t pad) {
    int final_pad = pad - strlen(str);

    while (final_pad-- > 0)
        putchar(' ');

    print(str);
}

/**
 * @brief Similar to stdlib's itoan, but instead of writing to buffer, prints.
 * @details Used by printf's "%i".
 * @param[in] num Number to print.
 */
static void printi(int64_t num) {
    /* Write '-' for negative numbers and convert it to positive */
    if (num < 0) {
        putchar('-');
        num = -num;
    }

    /* For more information about this loop, see stdlib's itoan. */
    for (int cur_digit = count_digits(num) - 1; cur_digit >= 0; cur_digit--)
        putchar((num / ipow(10, cur_digit)) % 10 + '0');
}

/**
 * @brief Print integer with padding.
 * @details Similar to printi(), but adds `digits(num) - pad` zeros before num.
 * Used for "%123d"
 * @param[in] num Number to print.
 * @param[in] pad Padding for the number.
 */
static void printi_n(int64_t num, uint32_t pad) {
    uint8_t sign = (num < 0) ? 1 : 0;
    if (sign)
        num = -num;

    char converted_num[21] = { 0 };
    itoa(converted_num, num);

    if (sign)
        putchar('-');

    int final_pad = (pad - strlen(converted_num)) - sign;
    while (final_pad-- > 0)
        putchar(' ');
    print(converted_num);
}

/**
 * @brief Print double.
 * @details Used by vprintf()
 * @param[in] num Number to print.
 * @param[in] decimals Number of decimal places to print.
 */
static void print_double(double num, uint32_t decimals) {
    /* First print the integer part using printi, and remove it from num for
     * only keeping decimals */
    int int_part = (int)num;
    printi(int_part);
    num -= int_part;

    if (num < 0)
        num = -num;

    /* If the double doesn't have decimals (12.0) */
    if (num == 0) {
        print(".0");
        return;
    }

    putchar('.');

    while (decimals-- > 0) {
        num *= 10;
        putchar((int)num + '0');
        num -= (int)num;
    }
}

/**
 * @brief Print double with decimal places and padding.
 * @details Used by vprintf()
 * @param[in] num Number to print.
 * @param[in] pad Padding. Same ass printi_n(). Used for "%123f"
 * @param[in] num Decimals. Used for "%.5f"
 */
static void print_double_n(double num, uint32_t pad, uint32_t decimals) {
    /* Print spaces for each digit we need to reach the padding */
    for (uint32_t i = 0; i < pad - digits_double(num, decimals); i++)
        putchar(' ');

    print_double(num, decimals);
}

/**
 * @brief Print integer in hexadecimal format (lowercase).
 * @details Does not support sign.
 * @param[in] num Number to print in hex format.
 */
static void printx(int64_t num) {
    if (num <= 0)
        print("0");

    /* max digits of an unsigned long */
    char hex_str[17] = { 0 };

    int tmp = 0;
    size_t i;
    for (i = 0; num > 0 && i < sizeof(hex_str) - 1; i++) {
        tmp = num % 16;
        num /= 16;

        /* Convert to char */
        tmp += (tmp < 10) ? '0' : 'a' - 10;

        hex_str[i] = tmp;
    }

    hex_str[i] = '\0';

    /* Reverse string and print */
    strrev(hex_str);
    print(hex_str);
}

/**
 * @brief Print integer in hexadecimal format with padding.
 * @details Similar to printx(), but adds `digits(num) - pad` zeros before num.
 * Used for "%123x", "%123X"
 * @param[in] num Number to print in hex format.
 * @param[in] pad Padding for the number.
 * @param[in] uppercase If true will use uppercase hex chars.
 */
static void printx_n(int64_t num, uint32_t pad, bool uppercase) {
    const char upper_c = uppercase ? 'A' : 'a';

    /* max digits of an unsigned long */
    char hex_str[17] = { 0 };

    int tmp = 0;
    size_t i;
    for (i = 0; num > 0 && i < sizeof(hex_str) - 1; i++) {
        tmp = num % 16;
        num /= 16;

        /* Convert to char */
        tmp += (tmp < 10) ? '0' : upper_c - 10;

        hex_str[i] = tmp;
    }

    hex_str[i] = '\0';

    /* Reverse string and print */
    strrev(hex_str);

    /* i is now the length of the final str */
    int final_pad = pad - i;
    while (final_pad-- > 0)
        putchar(' ');

    print(hex_str);
}

/**
 * @brief Print integer in hexadecimal format (uppercase).
 * @details Does not support sign.
 * @param[in] num Number to print in hex format.
 */
static void printX(int64_t num) {
    if (num <= 0)
        print("0");

    /* max digits of an unsigned long */
    char hex_str[17] = { 0 };

    int tmp = 0;
    size_t i;
    for (i = 0; num > 0 && i < sizeof(hex_str) - 1; i++) {
        tmp = num % 16;
        num /= 16;

        /* Convert to char */
        tmp += (tmp < 10) ? '0' : 'A' - 10;

        hex_str[i] = tmp;
    }

    hex_str[i] = '\0';

    /* Reverse string and print */
    strrev(hex_str);
    print(hex_str);
}

/**
 * @brief Print the address of the specified pointer in hex format.
 * @details Prints "(null)" if NULL.
 * @param[inout] ptr Pointer to print.
 */
static inline void printp(void* ptr) {
    if (ptr == NULL) {
        print("(null)");
    } else {
        print("0x");
        printX((uint32_t)ptr);
    }
}

int vprintf(const char* restrict fmt, va_list va) {
    int written = 0;

    while (*fmt != '\0') {
        /* If we get a '%', expect format, if not, just print normal char */
        if (*fmt == '%') {
            fmt++;
            if (written < INT_MAX)
                written++;
            else
                return -1; /**< @todo Set errno to EOVERFLOW */

            switch (*fmt) {
                case 'c':
                    putchar((char)va_arg(va, int));
                    break;
                case 's':
                    const char* va_str = va_arg(va, const char*);
                    print(va_str);

                    /* Add len to "written" if printing a string from va_list */
                    int va_strlen = strlen(va_str);
                    if (written + va_strlen < INT_MAX)
                        written += va_strlen;
                    else
                        return -1; /**< @todo Set errno to EOVERFLOW */

                    break;
                case 'd':
                    printi(va_arg(va, int));
                    break;
                case 'u':
                    printi(va_arg(va, unsigned int));
                    break;
                case 'f':
                    /* Floats get promoted to doubles when calling printf */
                    print_double(va_arg(va, double), _DEFAULT_DOUBLE_DECIMALS);
                    break;
                case 'x':
                    printx(va_arg(va, int));
                    break;
                case 'X':
                    printX(va_arg(va, int));
                    break;
                case 'p':
                    printp(va_arg(va, void*));
                    break;
                case '.': /* "%.123f" */
                    /* Skip the dot */
                    fmt++;

                    uint32_t dot_fmt_num = 0;
                    do {
                        dot_fmt_num *= 10;
                        dot_fmt_num += *fmt - '0';
                        fmt++;
                    } while (*fmt >= '0' && *fmt <= '9');

                    print_double(va_arg(va, double), dot_fmt_num);
                    break;
                case 'l':
                    /* Skip the first 'l' */
                    fmt++;

                    /* Check pattern  */
                    switch (*fmt) {
                        default: /* "%l?" -> "%ld" */
                            /* If it is an unknown char, subtract one and print
                             * int. We subtract one here because we want to
                             * analize the char on the next iteration, and it
                             * will get increased after the format switch. */
                            fmt--;

                            /* Adding this again is better than falling through
                             * to 'd' */
                            printi(va_arg(va, long));
                            break;
                        case 'd': /* "%ld" */
                            printi(va_arg(va, long));
                            break;
                        case 'u': /* "%lu" */
                            printi(va_arg(va, unsigned long));
                            break;
                        case 'f': /* "%lf" */
                            /* Same as "%f", see comment there */
                            print_double(va_arg(va, double),
                                         _DEFAULT_DOUBLE_DECIMALS);
                            break;
                        case 'x': /* "%lx" */
                            printx(va_arg(va, long));
                            break;
                        case 'X': /* "%lX" */
                            printX(va_arg(va, long));
                            break;
                        case 'l':
                            fmt++; /* Skip the second 'l' */

                            switch (*fmt) {
                                default: /* "%ll?" -> "%lld" */
                                    /* See previous comment on "%ld" */
                                    fmt--;
                                    printi(va_arg(va, long long));
                                    break;
                                case 'd': /* "%lld" */
                                    printi(va_arg(va, long long));
                                    break;
                                case 'u': /* "%llu" */
                                    printi(va_arg(va, unsigned long long));
                                    break;
                                case 'x': /* "%llx" */
                                    printx(va_arg(va, long long));
                                    break;
                                case 'X': /* "%llX" */
                                    printX(va_arg(va, long long));
                                    break;
                            } /* %lld switch */
                            break;
                    } /* %ld switch */

                    break;
                case '0': /* Not necessary */
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    /* "%123s", "%123d", ... */
                    /* Read all the numbers from the format */
                    uint32_t fmt_num = 0;
                    do {
                        fmt_num *= 10;
                        fmt_num += *fmt - '0';
                        fmt++;
                    } while (*fmt >= '0' && *fmt <= '9');

                    /*
                     * Now fmt points to the format char:
                     *  "%123lld"
                     *       ^
                     */
                    switch (*fmt) {
                        case 'd': /* "%123d" */
                            printi_n(va_arg(va, int), fmt_num);
                            break;
                        case 'u': /* "%123u" */
                            printi_n(va_arg(va, unsigned int), fmt_num);
                            break;
                        case 'f': /* "%123f" */
                            print_double_n(va_arg(va, double), fmt_num,
                                           _DEFAULT_DOUBLE_DECIMALS);
                            break;
                        case 'x': /* "%123x" */
                            printx_n(va_arg(va, int), fmt_num, false);
                            break;
                        case 'X': /* "%123X" */
                            printx_n(va_arg(va, int), fmt_num, true);
                            break;
                        case '.': /* "%123.5f" */
                            /* Skip the dot */
                            fmt++;

                            /* Not a digit after the dot */
                            if (*fmt < '0' || *fmt > '9')
                                break;

                            uint32_t dot_fmt_num = 0;
                            do {
                                dot_fmt_num *= 10;
                                dot_fmt_num += *fmt - '0';
                                fmt++;
                            } while (*fmt >= '0' && *fmt <= '9');

                            print_double_n(va_arg(va, double), fmt_num,
                                           dot_fmt_num);
                            break;
                        case 'l':  /* "%123ld", "%123lld", ... */
                            fmt++; /* Skip the first 'l' of "%123lld" */

                            switch (*fmt) {
                                default: /* "%123l?" -> "%123ld" */
                                    /* See previous comment on "%ld" */
                                    fmt--;
                                    printi_n(va_arg(va, long), fmt_num);
                                    break;
                                case 'd': /* "%123ld" */
                                    printi_n(va_arg(va, long), fmt_num);
                                    break;
                                case 'u': /* "%123lu" */
                                    printi_n(va_arg(va, unsigned long),
                                             fmt_num);
                                    break;
                                case 'f': /* "%123lf" */
                                    /* Same as "%123f". No "%123.5lf" format */
                                    print_double_n(va_arg(va, double), fmt_num,
                                                   _DEFAULT_DOUBLE_DECIMALS);
                                    break;
                                case 'x': /* "%123lx" */
                                    printx_n(va_arg(va, long), fmt_num, false);
                                    break;
                                case 'X': /* "%123lX" */
                                    printx_n(va_arg(va, long), fmt_num, true);
                                    break;
                                case 'l': /* "%123lld", "%123llx", ... */
                                    /* Skip the second 'l' of "%123lld" */
                                    fmt++;

                                    switch (*fmt) {
                                        default: /* "%123ll?" -> "%123lld" */
                                            /* See previous comment on "%ld" */
                                            fmt--;
                                            printi_n(va_arg(va, long long),
                                                     fmt_num);
                                            break;
                                        case 'd': /* "%123lld" */
                                            printi_n(va_arg(va, long long),
                                                     fmt_num);
                                            break;
                                        case 'u': /* "%123llu" */
                                            printi_n(
                                              va_arg(va, unsigned long long),
                                              fmt_num);
                                            break;
                                        case 'x': /* "%123llx" */
                                            printx_n(va_arg(va, long long),
                                                     fmt_num, false);
                                            break;
                                        case 'X': /* "%123llX" */
                                            printx_n(va_arg(va, long long),
                                                     fmt_num, true);
                                            break;
                                    } /* %123lld switch */
                                    break;
                            } /* %123ld switch */
                            break;
                        case 's': /* "%132s" */
                            prints_n(va_arg(va, const char*), fmt_num);
                            break;
                        default:
                            break;
                    } /* %123 switch */

                    break;
                case '%': /* "%%" -> "%" */
                    putchar(*fmt);
                    break;
                default:
                    /* If unknown fmt, print the % and the unknown char */
                    putchar('%');
                    putchar(*fmt);
                    break;
            } /* Main format char switch */
        } else {
            /** @todo Return value check? */
            putchar(*fmt);
        }

        fmt++;
        if (written < INT_MAX)
            written++;
        else
            return -1; /**< @todo Set errno to EOVERFLOW */
    }

    return written;
}

int puts(const char* str) {
    printf("%s\n", str);
    return 1; /* EOF means failure */
}

int printf(const char* restrict fmt, ...) {
    va_list va;
    va_start(va, fmt);

    int ret = vprintf(fmt, va);

    va_end(va);
    return ret;
}

int fprintf(FILE* restrict stream, const char* restrict fmt, ...) {
    va_list va;
    va_start(va, fmt);

    int ret = vfprintf(stream, fmt, va);

    va_end(va);
    return ret;
}

int vfprintf(FILE* restrict stream, const char* restrict fmt, va_list va) {
    /** @todo FILE struct and libc implementation */
    (void)stream;

    /* Just call normal vprintf */
    return vprintf(fmt, va);
}

int putchar(int c) {
    /** @todo Implement stdio and write syscalls */
    return c;
}

int getchar(void) {
    /** @todo Implement stdio and proper getchar */
    return EOF;
}
