
#ifndef _CTYPE_H
#define _CTYPE_H 1

#define isupper(c) (c >= 'A' && c <= 'Z')
#define islower(c) (c >= 'a' && c <= 'z')

/* tolower: if c is uppercase, returns its lowercase version */
int tolower(int c);

/* toupper: if c is lowercase, returns its uppercase version */
int toupper(int c);

#endif /* _CTYPE_H */

