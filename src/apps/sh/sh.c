
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include <kernel/color.h> /* color palette */

#include "sh.h"
#include "commands.h"

#define MAX_CMD_SZ 255
#define MAX_ARGC   100

#define LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

int sh_main(void) {
    int c = 0;

    char cur_cmd[MAX_CMD_SZ] = { 0 };
    char* argv[MAX_ARGC]     = { 0 };
    int argc                 = 0;

    while (!quit_sh) {
        fbc_setfore(COLOR_WHITE_B);
        printf("\n$ ");
        fbc_setfore(COLOR_GRAY);

        int cmd_pos = 0;
        while ((c = getchar()) != '\n' && cmd_pos < (int)(sizeof(cur_cmd) - 1))
            cur_cmd[cmd_pos++] = c;
        cur_cmd[cmd_pos] = '\0';

        /* For checking each word of the argv arr. Start as true to store the
         * first arg (program name) */
        bool was_space = true;

        /* Fill the argv array */
        for (cmd_pos = 0, argc = 0;
             cur_cmd[cmd_pos] != '\0' && argc < MAX_ARGC - 1; cmd_pos++) {
            if (cur_cmd[cmd_pos] == ' ') {
                /* First space after a word, set to null */
                if (!was_space)
                    cur_cmd[cmd_pos] = '\0';

                was_space = true;
            } else if (was_space) {
                /* First non-space after spaces, start of word */
                argv[argc++] = &cur_cmd[cmd_pos];
                was_space    = false;
            }
        }
        /* Last item of argv is 0 */
        argv[argc] = NULL;

#ifdef ARG_DEBUG
        printf("[dbg] argc: %d\n", argc);
        printf("[dbg] argv: [ ");
        int argv_i;
        for (argv_i = 0; argv_i < argc; argv_i++)
            printf("\"%s\" (%p), ", argv[argv_i], argv[argv_i]);
        printf("%p ]\n", argv[argv_i]); /* Last one is supposed to be NULL */
#endif

        bool valid_cmd = false;
        for (size_t i = 0; i < LENGTH(cmd_list); i++) {
            if (strcmp(argv[0], cmd_list[i].cmd) == 0) {
                /* Call the function with the args and store the return value */
                last_ret  = (*cmd_list[i].func)(argc, argv);
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
