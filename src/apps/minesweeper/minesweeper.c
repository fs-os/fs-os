
#include <stdio.h>
#include <curses.h>

/* minesweeper_main: entry point of the minesweeper */
int minesweeper_main(int argc, char** argv) {
    /* TODO */
    (void)argc;
    (void)argv;

    puts("Starting curses...");
    WINDOW* stdscr = initscr();

    printw("Hello, curses window!\n");

    move(5, 10);
    printw("Moved!\n");
    mvprintw(6, 10, "Moved... again!\n");
    move(7, 10);
    addch('A');
    mvaddch(8, 10, 'B');

    move(6, 16);
    clrtoeol();

    mvprintw(1, 0, "Input: ");
    while (getch() != '\n')
        ;
    printw("Got normal str. Enabling raw and noecho\n");

    raw();
    noecho();

    char c;
    while ((c = getch()) != '\n')
        if (c == 'c')
            clear();
        else
            printw("(%c)\n", c);

    endwin(stdscr);
    puts("Done!");

    return 0;
}

