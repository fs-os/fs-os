
/* sh: simple shell for the Free and Simple Operating System */

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include <kernel/framebuffer_console.h>
#include <kernel/color.h>

#include "sh.h"

#define MAX_CMD_SZ 255

#define LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

static void unk_cmd(void);
static void help_cmd(void);

static Command cmd_list[] = {
    { "help", "Show a list of commands", &help_cmd },
};

/* sh_main: main function of the shell */
int sh_main(void) {
    int c = 0;

    char cmd[MAX_CMD_SZ] = { 0 };

    for (;;) {
        fbc_setfore(COLOR_WHITE_B);
        printf("\n$ ");
        fbc_setfore(COLOR_GRAY);

        int cmd_pos = 0;
        while ((c = getchar()) != '\n')
            cmd[cmd_pos++] = c;
        cmd[cmd_pos] = '\0';

        bool valid_cmd = false;
        for (size_t i = 0; i < LENGTH(cmd_list); i++) {
            if (strcmp(cmd, cmd_list[i].cmd) == 0) {
                /* Call the function */
                (*cmd_list[i].func)();
                valid_cmd = true;
                break;
            }
        }

        /* If none of the cmds of cmd_list were valid, error */
        if (!valid_cmd)
            unk_cmd();
    }

    return 0;
}

static void unk_cmd(void) {
    puts("Unknown command... See \"help\" for more details.");
}

static void help_cmd(void) {
    fbc_setfore(COLOR_WHITE_B);
    puts("Command list:");

    for (size_t i = 0; i < LENGTH(cmd_list); i++) {
        fbc_setfore(COLOR_BLUE);
        printf("\t%s", cmd_list[i].cmd);
        fbc_setfore(COLOR_GRAY);
        printf(": %s\n", cmd_list[i].description);
    }

    fbc_setfore(COLOR_WHITE);
}

