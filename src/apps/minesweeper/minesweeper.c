
#include <stdio.h>
#include <curses.h>

/* minesweeper_main: entry point of the minesweeper */
int minesweeper_main(int argc, char** argv) {
    /* TODO */
    (void)argc;
    (void)argv;

    puts("Starting curses...");
    WINDOW* stdscr = initscr();

    puts("Hello, curses window!");

    printf("Input: ")
    while (getchar() != '\n')
        ;
    puts("Got normal str. Enabling raw and noecho");

    raw();
    noecho();

    char c;
    while ((c = getchar()) != '\n') 
        printf("(%c)\n", c);

    endwin(stdscr);
    puts("Done!");

    return 0;
}

