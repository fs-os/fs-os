
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
    /* Write '-' for negative numbers and convert number to positive */
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
        putchar('0');
    print(converted_num);
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
        putchar('0');

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
static void printp(void* ptr) {
    if (ptr == NULL) {
        print("(null)");
    } else {
        print("0x");
        printX((uint32_t)ptr);
    }
}

int puts(const char* str) {
    printf("%s\n", str);
    return 1; /* EOF means failure */
}

int printf(const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);

    int ret = vprintf(fmt, va);

    va_end(va);
    return ret;
}

int vprintf(const char* fmt, va_list va) {
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

                    /* If printing a string from va_list, add len to "written"
                     */
                    int va_strlen = strlen(va_str);
                    if (written + va_strlen < INT_MAX)
                        written += va_strlen;
                    else
                        return -1; /**< @todo Set errno to EOVERFLOW */

                    break;
                case 'd':
                    printi(va_arg(va, int));
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
                case 'l':
                    /* Check pattern. Not the best way but good enough for now
                     */
                    if (memcmp(fmt, "ld", 2) == 0) {
                        fmt++;                        /* The 'l' */
                        printi(va_arg(va, long int)); /* "%ld" */
                    } else if (memcmp(fmt, "lx", 2) == 0) {
                        fmt++;                        /* The 'l' */
                        printx(va_arg(va, long int)); /* "%lx" */
                    } else if (memcmp(fmt, "lX", 2) == 0) {
                        fmt++;                        /* The 'l' */
                        printX(va_arg(va, long int)); /* "%lX" */
                    } else if (memcmp(fmt, "ll", 2) == 0) {
                        fmt++; /* The 'l' */

                        if (memcmp(fmt, "lx", 2) == 0) {
                            fmt++;                             /* The 'x' */
                            printx(va_arg(va, long long int)); /* "%llx" */
                        } else if (memcmp(fmt, "lX", 2) == 0) {
                            fmt++;                             /* The 'X' */
                            printX(va_arg(va, long long int)); /* "%llX" */
                        } else if (memcmp(fmt, "ld", 2) == 0) {
                            fmt++;                             /* The 'd' */
                            printi(va_arg(va, long long int)); /* "%lld" */
                        } else {
                            printi(va_arg(va, long long int)); /* "%ll???" */
                        }
                    } else {
                        printi(va_arg(va, long int)); /* "%l???" */
                    }

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

                    switch (*fmt) {
                        case 'd': /* "%123d" */
                            printi_n(va_arg(va, int), fmt_num);
                            break;
                        case 'x':
                            printx_n(va_arg(va, int), fmt_num, false);
                            break;
                        case 'X':
                            printx_n(va_arg(va, int), fmt_num, true);
                            break;
                        case 'l': /* "%123ld", "%123llx", ... */
                            fmt++;

                            /* 123ld (long) */
                            switch (*fmt) {
                                default:
                                case 'd':
                                    printi_n(va_arg(va, long int), fmt_num);
                                    break;
                                case 'x':
                                    printx_n(va_arg(va, long int), fmt_num,
                                             false);
                                    break;
                                case 'X':
                                    printx_n(va_arg(va, long int), fmt_num,
                                             true);
                                    break;
                                case 'l':
                                    fmt++;

                                    /* 123lld (long long) */
                                    switch (*fmt) {
                                        default:
                                        case 'd':
                                            printi_n(va_arg(va, long long int),
                                                     fmt_num);
                                            break;
                                        case 'x':
                                            printx_n(va_arg(va, long long int),
                                                     fmt_num, false);
                                            break;
                                        case 'X':
                                            printx_n(va_arg(va, long long int),
                                                     fmt_num, true);
                                            break;
                                    }
                                    break;
                            }
                            break;
                        case 's': /* "%132s" */
                            prints_n(va_arg(va, const char*), fmt_num);
                            break;
                        default:
                            break;
                    }

                    break;
                case '%': /* "%%" -> "%" */
                default:
                    /* If unknown fmt, print the % and the unknown char */
                    putchar('%');
                    putchar(*fmt);
                    break;
            }
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

int putchar(int c) {
    /* TODO: Implement stdio and write syscalls */
    return c;
}

int getchar(void) {
    /* TODO: Implement stdio and proper getchar */
    return EOF;
}
