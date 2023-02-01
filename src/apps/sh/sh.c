
/* sh: simple shell for the Free and Simple Operating System */

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <time.h> /* sleep */

#include <kernel/color.h>               /* color palette */

#include "sh.h"
#include "commands.h"

#define MAX_CMD_SZ 255

#define LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

/* ------------------------------------------------------------------------------- */

/* sh_main: main function of the shell */
int sh_main(void) {
    int c = 0;

    char cur_cmd[MAX_CMD_SZ] = { 0 };

    while (!quit_sh) {
        fbc_setfore(COLOR_WHITE_B);
        printf("\n$ ");
        fbc_setfore(COLOR_GRAY);

        int cmd_pos = 0;
        while ((c = getchar()) != '\n' && cmd_pos < (int)(sizeof(cur_cmd) - 1))
            cur_cmd[cmd_pos++] = c;
        cur_cmd[cmd_pos] = '\0';

        bool valid_cmd = false;
        for (size_t i = 0; i < LENGTH(cmd_list); i++) {
            if (strcmp(cur_cmd, cmd_list[i].cmd) == 0) {
                /* Call the function */
                (*cmd_list[i].func)();
                valid_cmd = true;
                break;
            }
        }

        /* If none of the cmds of cmd_list were valid, error */
        if (!valid_cmd && cur_cmd[0] != '\0')
            cmd_unk();
    }

    return 0;
}

