
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h> /* sleep */

#include <kernel/framebuffer_console.h> /* fbc_setfore, fbc_clear */
#include <kernel/paging.h>              /* paging_show_map */
#include <kernel/heap.h>                /* heap_dump_headers */
#include <kernel/pit.h>                 /* pit_get_ticks */
#include <kernel/rtc.h>                 /* rtc_get_datetime */
#include <kernel/pcspkr.h>              /* pcspkr_beep */
#include <kernel/keyboard.h>            /* kb_setlayout, Layout */
#include <kernel/rand.h>                /* cpu_rand */
#include <kernel/multitask.h>           /* mt_newtask, mt_endtask */

#include "sh.h"

#include "../piano/piano.h"
#include "../minesweeper/minesweeper.h"
#include "../5x5/5x5.h"
#include "../../media/soviet_anthem.h"
#include "../../media/thunderstruck.h"

#define TEST_TITLE(...)         \
    fbc_setfore(COLOR_WHITE_B); \
    printf(__VA_ARGS__);        \
    putchar('\n');              \
    fbc_setfore(COLOR_GRAY);

/* Need tmp to remove '\0' from itoa */
#define PAD_ZEROS(n, p)                \
    do {                               \
        if (n < 10) {                  \
            *p       = '0';            \
            *(p + 1) = n + '0';        \
        } else {                       \
            const char tmp = *(p + 2); \
            itoa(p, n);                \
            *(p + 2) = tmp;            \
        }                              \
    } while (0);

/* Just used in cmd_test_libk */
#define SIGMAROUND(n) (double)(int)(n + 0.5)

#define LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

/* -------------------------------------------------------------------------- */

/* Used in sh_main and cmd_quit */
static bool quit_sh = false;
static int last_ret = 0;

/* Need to declare them here because the array needs the functions, but the
 * functions also need the array */
static int cmd_unk();
static int cmd_help();
static int cmd_quit();
static int cmd_last();
static int cmd_clear();
static int cmd_ref();
static int cmd_loadkeys(int argc, char** argv);
static int cmd_ticks();
static int cmd_date();
static int cmd_timer(int argc, char** argv);
static int cmd_beep(int argc, char** argv);
static int cmd_metronome(int argc, char** argv);
/* piano_main */
/* piano_random */
/* minesweeper_main */
/* main_5x5 */
static int cmd_page_map();
static int cmd_heap_headers();
static int cmd_test_libk();
static int cmd_test_multitask();
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
      "loadkeys",
      "Changes the current keyboard layout",
      &cmd_loadkeys,
    },
    {
      "ticks",
      "Print the current tick count since boot (ms)",
      &cmd_ticks,
    },
    {
      "date",
      "Display current date and time",
      &cmd_date,
    },
    {
      "timer",
      "Simple timer command (Wrapper for time.h functions)",
      &cmd_timer,
    },
    {
      "beep",
      "Beep through the pc speaker (optional frequency and duration)",
      &cmd_beep,
    },
    {
      "metronome",
      "Start the metronome",
      &cmd_metronome,
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
      "minesweeper",
      "Simple terminal minesweeper",
      &minesweeper_main,
    },
    {
      "5x5",
      "Simple 5x5 game",
      &main_5x5,
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
      "test_multitask",
      "Test multitasking with 3 threads",
      &cmd_test_multitask,
    },
    {
      "play",
      "Play a song using the pc speaker",
      &cmd_play,
    },
};

/* -------------------------------------------------------------------------------
 */

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
    return last_ret; /* Keep the same exit code in case we want to call it twice
                      */
}

static int cmd_clear() {
    fbc_clear();
    fbc_refresh_raw();

    return 0;
}

static int cmd_ref() {
    fbc_refresh();
    return 0;
}

static int cmd_loadkeys(int argc, char** argv) {
    if (argc <= 1 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
        printf("Usage:\n"
               "\t%s --help    - Show this help\n"
               "\t%s --list    - List the layouts\n"
               "\t%s [layout]  - Load the specified layout\n",
               argv[0], argv[0], argv[0]);
        return 1;
    }

    typedef struct {
        const char* name;
        const Layout* layout;
    } layout_pair_t;

    layout_pair_t layouts[] = {
        { "us", &us_layout },
        { "es", &es_layout },
    };

    if (strcmp(argv[1], "--list") == 0) {
        fbc_setfore(COLOR_WHITE_B);
        puts("Available layouts:");
        fbc_setfore(COLOR_WHITE);

        for (size_t i = 0; i < LENGTH(layouts); i++)
            printf("- %s\n", layouts[i].name);

        return 0;
    } else {
        for (size_t i = 0; i < LENGTH(layouts); i++) {
            if (strcmp(argv[1], layouts[i].name) == 0) {
                kb_setlayout(layouts[i].layout);
                return 0;
            }
        }
    }

    printf("Invalid option \"%s\"\n"
           "Usage:\n"
           "\t%s --help    - Show this help\n"
           "\t%s --list    - List the layouts\n"
           "\t%s [layout]  - Load the specified layout\n",
           argv[1], argv[0], argv[0], argv[0]);
    return 1;
}

static int cmd_ticks() {
    const uint64_t t = pit_get_ticks();
    printf("%lld (%llds)\n", t, t / 1000);
    return 0;
}

static int cmd_date() {
    static char date_str[] = "00/00/00 - 00:00:00";
    const DateTime now     = rtc_get_datetime();

    PAD_ZEROS(now.date.d, &date_str[0]);
    PAD_ZEROS(now.date.m, &date_str[3]);
    PAD_ZEROS(now.date.y, &date_str[6]);
    PAD_ZEROS(now.time.h, &date_str[11]);
    PAD_ZEROS(now.time.m, &date_str[14]);
    PAD_ZEROS(now.time.s, &date_str[17]);

    fbc_setfore(COLOR_WHITE_B);
    printf("Date: ");
    fbc_setfore(COLOR_GRAY);
    puts(date_str);

    fbc_setfore(COLOR_WHITE_B);
    printf("Epoch (Not acurate): ");
    fbc_setfore(COLOR_GRAY);
    printf("%lu\n", time(NULL));

    fbc_setfore(COLOR_WHITE);
    return 0;
}

static int cmd_timer(int argc, char** argv) {
    if (argc <= 1 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
        printf("Usage:\n"
               "\t%s --help  - Show this help\n"
               "\t%s start   - Start the timer\n"
               "\t%s stop    - Print the time since we started the timer\n",
               argv[0], argv[0], argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "start") == 0) {
        timer_start();
    } else if (strcmp(argv[1], "stop") == 0) {
        printf("%llums\n", timer_stop());
    } else {
        printf("Invalid option \"%s\"\n"
               "Usage:\n"
               "\t%s --help  - Show this help\n"
               "\t%s start   - Start the timer\n"
               "\t%s stop    - Print the time since we started the timer\n",
               argv[1], argv[0], argv[0], argv[0]);
        return 1;
    }

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

static int cmd_metronome(int argc, char** argv) {
    const int beep_duration = 10;  /* ms */
    uint32_t freq           = 150; /* hz for the pcspkr */
    uint32_t bpm            = 60;  /* beats per minute */

    /*** Argument parsing ***/
    bool arg_error = false;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-f") || !memcmp(argv[i], "--freq", 5)) {
            if (i == argc - 1) {
                printf("Not enough arguments for \"%s\"\n", argv[i]);
                arg_error = true;
                break;
            }

            i++;
            freq = atoi(argv[i]);
            if (freq < 1) {
                printf("Invalid frequency format for \"%s\".\n", argv[i - 1]);
                arg_error = true;
                break;
            }
        } else if (!strcmp(argv[i], "-b") || !strcmp(argv[i], "--bpm")) {
            if (i == argc - 1) {
                printf("Not enough arguments for \"%s\"\n", argv[i]);
                arg_error = true;
                break;
            }

            i++;
            bpm = atoi(argv[i]);
            if (bpm < 1) {
                printf("Invalid BPM format for \"%s\".\n", argv[i - 1]);
                arg_error = true;
                break;
            }
        } else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            arg_error = true;
            break;
        }
    }

    if (arg_error) {
        printf("Usage:\n"
               "\t%s --help     - Show this help\n"
               "\t%s -f [freq]  - Start specified frequency\n"
               "\t%s -b [bpm]   - Start specified beats per minute\n",
               argv[0], argv[0], argv[0]);
        return 1;
    }
    /*** End argument parsing ***/

    /* Get ms delay from bpm */
    const uint32_t ms_delay = 1000 * 60 / bpm;

    Beep beep = {
        .freq   = freq,
        .ms_len = beep_duration,
    };

    printf("Running metronome at %luhz and %lu BPM.\n"
           "Hold \'q\' to quit...\n",
           freq, bpm);

    while (!kb_held('q')) {
        pcspkr_beep_custom(beep);
        sleep_ms(ms_delay);
    }

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
    printf("memcmp(\"12345\", \"12345\", 5) -> %d\n",
           memcmp("12345", "12345", 5));

    /* More than one line for the null terminator */
    printf("memset(buf, 'h', 5) -> ");
    memset(buf, 'h', 5);
    buf[5] = '\0';
    puts(buf);

    printf("memcpy(&buf[5], &buf[0], 5) -> ");
    memcpy(&buf[5], &buf[0], 5);
    buf[10] = '\0';
    puts(buf);

    printf("srand(time(NULL))\n");
    srand(time(NULL));
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

    TEST_TITLE("\nTesting float formats");
    printf("(rand() %% 10) + 1.3f = %f\n", (rand() % 10) + 1.3f);
    printf("654.12345678f (\"%%.3f\") -> \"%.3f\"\n", 654.12345678f);
    printf("654.12345678f (\"%%10.4f\") -> \"%10.4f\"\n", 654.12345678f);
    printf("SIGMAROUND(1.4) = %f\n", SIGMAROUND(1.4));
    printf("SIGMAROUND(1.5) = %f\n", SIGMAROUND(1.5));

    TEST_TITLE("\nTesting time.h functions");
    printf("Hello, ");
    sleep(1);
    printf("world!\n");

    return 0;
}

#define MT_TEST_ITERS 3
#define MT_TEST_DELAY 100

static void multitask_test0(void) {
    Ctx* self = mt_gettask();

    for (int i = 0; i <= MT_TEST_ITERS; i++) {
        printf("%s: %.2f\n", self->name, i + 0.1f);
        sleep_ms(MT_TEST_DELAY);
        mt_switch(self->next);
    }
}

static void multitask_test1(void) {
    Ctx* self = mt_gettask();

    for (int i = 0; i <= MT_TEST_ITERS; i++) {
        printf("%s: %.2f\n", self->name, i + 0.2f);
        sleep_ms(MT_TEST_DELAY);
        mt_switch(self->next);
    }
}

static void multitask_test2(void) {
    Ctx* self = mt_gettask();

    for (int i = 0; i <= MT_TEST_ITERS; i++) {
        printf("%s: %.2f\n", self->name, i + 0.3f);
        sleep_ms(MT_TEST_DELAY);
        mt_switch(self->next);
    }
}

static int cmd_test_multitask() {
    TEST_TITLE("Testing multitasking with %d iterations and %dms of delay",
               MT_TEST_ITERS, MT_TEST_DELAY);

    /*
     * When creating more than 1 task, the last tasks added will be placed after
     * the current task:
     *   - First, task2 will be placed after the current task
     *   - Second, task1 will be placed after the current task, so between the
     *     current task and task2
     *   - Third, task0 will be placed after the current task, so between the
     *     current task and task1
     *
     * So in the end:
     *     [cur_task] -> [task0] -> [task1] -> [task2]
     *
     * For more information, call dump_task_list() after creating the tasks.
     */
    Ctx* task2 = mt_newtask("task2", multitask_test2);
    Ctx* task1 = mt_newtask("task1", multitask_test1);
    Ctx* task0 = mt_newtask("task0", multitask_test0);

    Ctx* self = mt_gettask();

    for (int i = 0; i <= MT_TEST_ITERS; i++) {
        printf("%s: %.2f\n", self->name, (float)i);
        sleep_ms(MT_TEST_DELAY);
        mt_switch(self->next);
    }

    /* Order does not matter */
    mt_endtask(task0);
    mt_endtask(task1);
    mt_endtask(task2);

    return 0;
}

static int cmd_play(int argc, char** argv) {
    if (argc <= 1 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
        printf("Usage:\n"
               "\t%s --help       - Show this help\n"
               "\t%s --list       - List the available songs\n"
               "\t%s <song name>  - Play the specified song\n",
               argv[0], argv[0], argv[0]);
        return 1;
    }

    typedef struct {
        const char* name;
        void (*func)(void);
    } song_pair_t;

    song_pair_t songs[] = {
        { "soviet", &play_soviet_anthem },
        { "thunder", &play_thunderstruck },
        { "thunderstruck", &play_thunderstruck },
    };

    if (strcmp(argv[1], "--list") == 0) {
        fbc_setfore(COLOR_WHITE_B);
        puts("Available songs:");
        fbc_setfore(COLOR_WHITE);

        for (size_t i = 0; i < LENGTH(songs); i++)
            printf("- %s\n", songs[i].name);

        return 0;
    } else {
        for (size_t i = 0; i < LENGTH(songs); i++) {
            if (strcmp(argv[1], songs[i].name) == 0) {
                /* Call the function */
                (*songs[i].func)();

                return 0;
            }
        }
    }

    printf("Invalid song name: \"%s\"\n", argv[1]);
    return 1;
}
