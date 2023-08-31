
#include <stdint.h>
#include <stddef.h>
#include <string.h>

size_t strlen(const char* str) {
    size_t ret = 0;

    while (str[ret] != '\0')
        ret++;

    return ret;
}

char* strrev(char* str) {
    const int len = strlen(str);
    char c        = 0;

    /*
     * Go from str[0] to str[len/2] and replace each char with the char from the
     * other half:
     * s = [ H E L L O ] => [ O L L E H ]
     *       | |   | |
     *       | ----- |
     *       |-------|
     */
    for (int i = 0; i < len / 2 && str[i] != '\0'; i++) {
        c = str[i];

        /* -1 to convert length to idx */
        str[i]           = str[len - 1 - i];
        str[len - 1 - i] = c;
    }

    return str;
}

int memcmp(const void* a, const void* b, size_t sz) {
    uint8_t* ap = (uint8_t*)a;
    uint8_t* bp = (uint8_t*)b;

    while (sz-- > 0) {
        if (*ap < *bp)
            return -1;
        else if (*ap > *bp)
            return 1;

        ap++;
        bp++;
    }

    return 0;
}

void* memset(void* ptr, int val, size_t sz) {
    uint8_t* p = (uint8_t*)ptr;

    while (sz-- > 0)
        *p++ = (uint8_t)val;

    return ptr;
}

void* memcpy(void* restrict dst, const void* restrict src, size_t sz) {
    uint8_t* dp = (uint8_t*)dst;
    uint8_t* sp = (uint8_t*)src;

    while (sz-- > 0)
        *dp++ = *sp++;

    return dst;
}

int strcmp(const char* a, const char* b) {
    /* See section 5.5 of TCPL (K&R) */
    while (*a == *b) {
        /* Because of the while condition, we know *b is '\0' too */
        if (*a == '\0')
            return 0;

        /* Only increase if they match */
        a++;
        b++;
    }

    return *a - *b;
}
