
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h> /* sleep */

#include <kernel/framebuffer_console.h> /* fbc_setfore, fbc_clear */
#include <kernel/paging.h>              /* paging_show_map */
#include <kernel/heap.h>                /* heap_dump_headers */
#include <kernel/rtc.h>                 /* rtc_get_datetime */
#include <kernel/pcspkr.h>              /* pcspkr_beep */
#include <kernel/rand.h>                /* check_rdseed, check_rdrand, cpu_rand */

#include "sh.h"

#include "../piano/piano.h"
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
static int last_ret = 0;

/* Need to declare them here because the array needs the functions, but the functions
 * also need the array */
static int cmd_unk();
static int cmd_help();
static int cmd_quit();
static int cmd_last();
static int cmd_ping();
static int cmd_beep(int argc, char** argv);
static int cmd_clear();
static int cmd_ref();
static int cmd_date();
static int cmd_page_map();
static int cmd_heap_headers();
static int cmd_test_libk();
static int cmd_play(int argc, char** argv);

/*
 * Structure of the array:
 *   command name, description, command function pointer
 * For adding commands, see:
 *   https://github.com/fs-os/fs-os/commit/b61e2b7d7934d1f0f58442152bd0563c91439a52
 */
static Command cmd_list[] = {
    {
      "help",
      "Show a list of commands",
      &cmd_help,
    },
    {
      "quit",
      "Permanently exit the shell",
      &cmd_quit,
    },
    {
      "last",
      "Print the exit code of the last command",
      &cmd_last,
    },
    {
      "ping",
      "Simple test command",
      &cmd_ping,
    },
    {
      "beep",
      "Beep through the pc speaker (optional frequency and duration)",
      &cmd_beep,
    },
    {
      "clear",
      "Clear the console",
      &cmd_clear,
    },
    {
      "ref",
      "Refresh the console",
      &cmd_ref,
    },
    {
      "piano",
      "Play the piano through the pc speaker",
      &piano_main,
    },
    {
      "piano_random",
      "Random piano through the pc speaker",
      &piano_random,
    },
    {
      "date",
      "Display current date and time",
      &cmd_date,
    },
    {
      "page_map",
      "Display the page director and page table layout",
      &cmd_page_map,
    },
    {
      "heap_headers",
      "Dump the alloc headers",
      &cmd_heap_headers,
    },
    {
      "test_libk",
      "Test the kernel standard lib",
      &cmd_test_libk,
    },
    {
      "play",
      "Play a song using the pc speaker",
      &cmd_play,
    },
};

/* ------------------------------------------------------------------------------- */

static int cmd_unk() {
    puts("Unknown command... See \"help\" for more details.");
    return 1;
}

static int cmd_help() {
    fbc_setfore(COLOR_WHITE_B);
    puts("Command list:");

    for (size_t i = 0; i < LENGTH(cmd_list); i++) {
        fbc_setfore(COLOR_BLUE);
        printf("\t%s", cmd_list[i].cmd);
        fbc_setfore(COLOR_GRAY);
        printf(": %s\n", cmd_list[i].description);
    }

    fbc_setfore(COLOR_WHITE);

    return 0;
}

static int cmd_quit() {
    puts("Goodbye.");
    quit_sh = true;
    return 0;
}

static int cmd_last() {
    printf("Last exit code: %d\n", last_ret);
    return last_ret; /* Keep the same exit code in case we want to call it twice */
}

static int cmd_ping() {
    puts("pong");
    return 0;
}

static int cmd_beep(int argc, char** argv) {
    /* Default beep */
    Beep beep_info = {
        1000,
        20,
    };

    if (argc <= 1) {
        pcspkr_beep_custom(beep_info);
        return 0;
    }
    /* 1st arg, freq */
    beep_info.freq = atoi(argv[1]);

    /* 2nd arg, duration (optional) */
    if (argc > 2)
        beep_info.ms_len = atoi(argv[2]);

    if (beep_info.freq == 0 || beep_info.ms_len == 0) {
        printf("Invalid parameters.\n"
               "Usage:\n"
               "\t%s [ms frequency] [ms duration]\n",
               argv[0]);
        return 1;
    }

    pcspkr_beep_custom(beep_info);

    return 0;
}

static int cmd_clear() {
    fbc_clear();
    fbc_refresh();

    return 0;
}

static int cmd_ref() {
    fbc_refresh();
    return 0;
}

static int cmd_date() {
    const DateTime now = rtc_get_datetime();

    fbc_setfore(COLOR_WHITE_B);
    printf("Date: ");
    fbc_setfore(COLOR_GRAY);
    printf("%2d/%2d/%2d - %2d:%2d:%2d\n", now.date.d, now.date.m, now.date.y,
           now.time.h, now.time.m, now.time.s);
    fbc_setfore(COLOR_WHITE);

    return 0;
}

static int cmd_page_map() {
    paging_show_map();
    return 0;
}

static int cmd_heap_headers() {
    heap_dump_headers();
    return 0;
}

static int cmd_test_libk() {
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

    printf("srand(1337)\n");
    srand(1337);
    printf("rand() -> %d\n", rand());
    printf("rand() -> %d\n", rand());

    if (!check_rdseed() && !check_rdrand()) {
        fbc_setfore(COLOR_RED);
        puts("rdseed and rdrand not supported, skipping...");
        fbc_setfore(COLOR_GRAY);
    } else {
        printf("cpu_rand() -> %ld\n", cpu_rand());
        printf("cpu_rand() -> %ld\n", cpu_rand());
    }

    TEST_TITLE("\nTesting time.h functions");
    printf("Hello, ");
    sleep(2);
    printf("world!\n");

    return 0;
}

static int cmd_play(int argc, char** argv) {
    if (argc < 2) {
        printf("Invalid parameters.\n"
               "Usage:\n"
               "\t%s <song name>\n",
               argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "soviet") == 0) {
        play_soviet_anthem();
    } else if (strcmp(argv[1], "thunder") == 0 ||
               strcmp(argv[1], "thunderstruck") == 0) {
        play_thunderstruck();
    } else {
        printf("Invalid song name: \"%s\"\n", argv[1]);
        return 1;
    }

    return 0;
}

