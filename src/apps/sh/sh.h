
#ifndef _APPS_SH_H
#define _APPS_SH_H

/* Command: pair of command and void function to be called. */
typedef struct {
    char* cmd;
    char* description;
    int (*func)(int argc, char** argv);
} Command;

/* sh_main: main function of the shell */
int sh_main(void);

#endif /* _APPS_SH_H */

