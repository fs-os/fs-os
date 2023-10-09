
#ifndef APPS_SH_H_
#define APPS_SH_H_ 1

/**
 * @brief Simple command structure for the shell
 */
typedef struct {
    /** @brief Actual command string for the shell */
    char* cmd;

    /** @brief Description for sh's *help* command */
    char* description;

    /** @brief Pointer to the command's entry point */
    int (*func)(int argc, char** argv);
} Command;

/**
 * @brief Main function of the shell
 * @return Exit code
 */
int sh_main(void);

#endif /* APPS_SH_H_ */
