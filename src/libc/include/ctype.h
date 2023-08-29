
#ifndef _CTYPE_H
#define _CTYPE_H 1

/**
 * @brief Returns true if `c` is uppercase.
 * @param[in] c Character to check
 * @return True of uppercase
 */
static inline bool isupper(int c) {
    return c >= 'A' && c <= 'Z';
}

/**
 * @brief Returns true if `c` is lowercase.
 * @param[in] c Character to check
 * @return True of lowercase
 */
static inline bool islower(c) {
    return c >= 'a' && c <= 'z';
}

/**
 * @brief If c is uppercase, returns its lowercase version.
 * @param[in] c Uppercase char
 * @return Lowercase version of c
 */
int tolower(int c);

/**
 * @brief If c is lowercase, returns its uppercase version.
 * @param[in] c Lowercase char
 * @return Uppercase version of c
 */
int toupper(int c);

#endif /* _CTYPE_H */
