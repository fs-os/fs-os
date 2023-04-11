
#ifndef _CTYPE_H
#define _CTYPE_H 1

/**
 * @def isupper
 * @brief Returns true if `c` is uppercase.
 */
#define isupper(c) (c >= 'A' && c <= 'Z')

/**
 * @def islower
 * @brief Returns true if `c` is lowercase.
 */
#define islower(c) (c >= 'a' && c <= 'z')

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
