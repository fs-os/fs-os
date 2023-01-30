
#include <kernel/framebuffer_console.h> /* fbc_setfore */
#include <kernel/heap.h>                /* heap_dump_headers */
#include <kernel/rtc.h>                 /* rtc_get_datetime */

#include "sh.h"

#include "../../kernel/media/soviet_anthem.h"
#include "../../kernel/media/thunderstruck.h"

#define TEST_TITLE(s)               \
    {                               \
        fbc_setfore(COLOR_WHITE_B); \
        puts(s);                    \
        fbc_setfore(COLOR_GRAY);    \
    }

#define LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

/* ------------------------------------------------------------------------------- */

/* Used in sh_main and cmd_quit */
static bool quit_sh = false;

/* Need to declare them here because the array needs the functions, but the functions
 * also need the array */
static void cmd_unk(void);
static void cmd_help(void);
static void cmd_quit(void);
static void cmd_ping(void);
static void cmd_date(void);
static void cmd_heap_headers(void);
static void cmd_test_libk(void);
static void cmd_play_soviet(void);
static void cmd_play_thunder(void);

/*
 * Structure of the array:
 *   command name, description, command function pointer
 * For adding commands, see:
 *   https://github.com/fs-os/fs-os/commit/b61e2b7d7934d1f0f58442152bd0563c91439a52
 */
/* TODO: arg support (for sleep for example) */
static Command cmd_list[] = {
    { "help", "Show a list of commands", &cmd_help },
    { "quit", "Permanently exit the shell", &cmd_quit },
    { "date", "Display current date and time", &cmd_date },
    { "ping", "Simple test command", &cmd_ping },
    { "heap_headers", "Dump the alloc headers", &cmd_heap_headers },
    { "test_libk", "Test the kernel standard lib", &cmd_test_libk },
    { "play_soviet", "Play the soviet anthem using the pc speaker",
      &cmd_play_soviet },
    { "play_thunder", "Play thunderstruck using the pc speaker", &cmd_play_thunder },
};

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

static void cmd_ping(void) {
    puts("pong");
}

static void cmd_date(void) {
    const DateTime now = rtc_get_datetime();

    fbc_setfore(COLOR_WHITE_B);
    printf("Date: ");
    fbc_setfore(COLOR_GRAY);
    printf("%2d/%2d/%2d - %2d:%2d:%2d\n", now.date.d, now.date.m, now.date.y,
           now.time.h, now.time.m, now.time.s);
    fbc_setfore(COLOR_WHITE);
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

