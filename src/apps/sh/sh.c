
/* sh: simple shell for the Free and Simple Operating System */

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <time.h> /* sleep */

#include <kernel/color.h>               /* color palette */
#include <kernel/framebuffer_console.h> /* fbc_setfore */
#include <kernel/heap.h>                /* heap_dump_headers */

#include "sh.h"

#include "../../kernel/media/soviet_anthem.h"
#include "../../kernel/media/thunderstruck.h"

#define MAX_CMD_SZ 255

#define LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

#define TEST_TITLE(s)               \
    {                               \
        fbc_setfore(COLOR_WHITE_B); \
        puts(s);                    \
        fbc_setfore(COLOR_GRAY);    \
    }

/* ------------------------------------------------------------------------------- */

static bool quit_sh = false;

static void cmd_unk(void);
static void cmd_help(void);
static void cmd_quit(void);
static void cmd_heap_headers(void);
static void cmd_test_libk(void);
static void cmd_play_soviet(void);
static void cmd_play_thunder(void);

static Command cmd_list[] = {
    { "help", "Show a list of commands", &cmd_help },
    { "quit", "Permanently exit the shell", &cmd_quit },
    { "heap_headers", "Dump the alloc headers", &cmd_heap_headers },
    { "test_libk", "Test the kernel standard lib", &cmd_test_libk },
    { "play_soviet", "Play the soviet anthem using the pc speaker",
      &cmd_play_soviet },
    { "play_thunder", "Play thunderstruck using the pc speaker", &cmd_play_thunder },
};

/* sh_main: main function of the shell */
int sh_main(void) {
    int c = 0;

    char cmd[MAX_CMD_SZ] = { 0 };

    while (!quit_sh) {
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
            cmd_unk();
    }

    return 0;
}

/* ------------------------------------------------------------------------------- */

static void cmd_unk(void) {
    puts("Unknown command... See \"help\" for more details.");
}

static void cmd_help(void) {
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

static void cmd_quit(void) {
    quit_sh = true;
}

static void cmd_heap_headers(void) {
    heap_dump_headers();
}

static void cmd_test_libk(void) {
    TEST_TITLE("\nTesting stdlib.h, string.h and stdio.h functions");

    char buf[255] = { 0 };

    printf("strlen(\"abcd\") -> %ld\n", strlen("abcd"));
    printf("memcmp(\"abcd\", \"abca\", 4) -> %d\n", memcmp("abcd", "abc1", 4));
    printf("memcmp(\"abcd\", \"abce\", 4) -> %d\n", memcmp("abcd", "abce", 4));
    printf("memcmp(\"12345\", \"12345\", 5) -> %d\n", memcmp("12345", "12345", 5));

    /* More than one line for the null terminator */
    printf("memset(buf, 'h', 5) -> ");
    memset(buf, 'h', 5);
    buf[5] = '\0';
    puts(buf);

    printf("memcpy(&buf[5], &buf[0], 5) -> ");
    memcpy(&buf[5], &buf[0], 5);
    buf[10] = '\0';
    puts(buf);

    TEST_TITLE("\nTesting time.h functions");
    printf("Hello, ");
    sleep(2);
    printf("world!\n");
}

static void cmd_play_soviet(void) {
    play_soviet_anthem();
}

static void cmd_play_thunder(void) {
    play_thunderstruck();
}

