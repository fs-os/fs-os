
/* TODO: Remove unused functions from libk? */

#include <stddef.h>
#include <string.h>

/* strlen: return length of str without null terminator */
size_t strlen(const char* str) {
    size_t ret = 0;

    while (str[ret] != '\0')
        ret++;

    return ret;
}

/* memcmp: compare the first "sz" bytes of 2 pointers. Returns 1 if in the first
 * mismatch, a is greater than b, or -1 if b is greater than a. If all the "sz" bytes
 * in a and b are the same, returns 0. */
int memcmp(const void* a, const void* b, size_t sz) {
    unsigned char* ap = (unsigned char*)a;
    unsigned char* bp = (unsigned char*)b;

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

/* memset: sets "sz" bytes of "ptr" to "value". Returns "ptr" */
void* memset(void* ptr, int val, size_t sz) {
    unsigned char* p = (unsigned char*)ptr;

    while (sz-- > 0)
        *p++ = (unsigned char)val;

    return ptr;
}

/* memcpy: copies "sz" bytes of "src" into "dst". Returns "dst" */
void* memcpy(void* restrict dst, const void* restrict src, size_t sz) {
    unsigned char* dp = (unsigned char*)dst;
    unsigned char* sp = (unsigned char*)src;

    while (sz-- > 0)
        *dp++ = *sp++;

    return dst;
}

