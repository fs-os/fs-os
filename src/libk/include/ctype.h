
#ifndef CTYPE_H_
#define CTYPE_H_ 1

#include <stdbool.h>

/**
 * @brief If c is uppercase, returns its lowercase version
 * @param[in] c Uppercase char
 * @return Lowercase version of c
 */
int tolower(int c);

/**
 * @brief If c is lowercase, returns its uppercase version
 * @param[in] c Lowercase char
 * @return Uppercase version of c
 */
int toupper(int c);

/**
 * @brief Returns true if `c` is uppercase
 * @param[in] c Character to check
 * @return True if uppercase
 */
static inline bool isupper(int c) {
    return c >= 'A' && c <= 'Z';
}

/**
 * @brief Returns true if `c` is lowercase
 * @param[in] c Character to check
 * @return True if lowercase
 */
static inline bool islower(int c) {
    return c >= 'a' && c <= 'z';
}

/**
 * @brief Returns true if `c` is an alphabetic character
 * @param[in] c Character to check
 * @return True if digit
 */
static inline bool isalpha(int c) {
    return isupper(c) || islower(c);
}

/**
 * @brief Returns true if `c` is a digit
 * @param[in] c Character to check
 * @return True if digit
 */
static inline bool isdigit(int c) {
    return c >= '0' && c <= '9';
}

/**
 * @brief Returns true if `c` is alphanumeric
 * @param[in] c Character to check
 * @return True if alphanumeric
 */
static inline bool isalnum(int c) {
    return isalpha(c) || isdigit(c);
}

/**
 * @brief Returns true if `c` is tab or space
 * @param[in] c Character to check
 * @return True if blank character
 */
static inline bool isblank(int c) {
    return c == ' ' || c == '\t';
}

#endif /* CTYPE_H_ */
