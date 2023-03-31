
#include <stdio.h>
#include <curses.h>

/* minesweeper_main: entry point of the minesweeper */
int minesweeper_main(int argc, char** argv) {
    /* TODO */
    (void)argc;
    (void)argv;

    puts("start");
    WINDOW* stdscr = initscr();

    puts("Hello, curses window!\nPress any key to continue...");

    while (getchar() != '\n')
        ;

    endwin(stdscr);
    puts("done");

    return 0;
}

