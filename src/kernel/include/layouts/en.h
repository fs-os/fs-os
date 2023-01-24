
#include <kernel/keyboard.h> /* layout */

static const layout en_layout = {
    .def =
      (char[]){
        0,                                                             /**/
        0x1b,                                                          /* esc */
        '1',  '2',  '3', '4', '5', '6', '7', '8', '9', '0', '-',  '=', /**/
        '\b', '\t',                                                    /**/
        'q',  'w',  'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[',  ']', '\n', /**/
        0, /* left ctrl */
        'a',  's',  'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', /**/
        0,                                                       /* left shift */
        '\\', 'z',  'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', /**/
        0,                                                       /* right shift */
        '*',                                                     /**/
        0,                                                       /* alt */
        ' ',                                                     /* space */
        0,                                                       /* capslock */
        0,    0,    0,   0,   0,   0,   0,   0,   0,   0,        /* F1 .. F10 */
        0,                                                       /* numlock */
        0,                                                       /* scroll Lock */
        0,                                                       /* home */
        0,                                                       /* up arrow */
        0,                                                       /* page up */
        '-',                                                     /**/
        0,                                                       /* left arrow */
        0,                                                       /**/
        0,                                                       /* right arrow */
        '+',                                                     /**/
        0,                                                       /* end */
        0,                                                       /* down arrow */
        0,                                                       /* page down */
        0,                                                       /* insert */
        0,                                                       /* delete */
        0,    0,    0,                                           /**/
        0,                                                       /* f11 key */
        0,                                                       /* f12 key */
        0, /* all other keys are undefined */
      },
    .shift =
      (char[]){
        0,                                                             /**/
        0x1b,                                                          /* esc */
        '!',  '@',  '#', '$', '%', '^', '&', '*', '(', ')', '_',  '+', /**/
        '\b', '\t',                                                    /**/
        'Q',  'W',  'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{',  '}', '\n', /**/
        0, /* left ctrl */
        'A',  'S',  'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', /**/
        0,                                                       /* left shift */
        '|',  'Z',  'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', /**/
        0,                                                       /* right shift */
        '*',                                                     /**/
        0,                                                       /* alt */
        ' ',                                                     /* space */
        0,                                                       /* capslock */
        0,    0,    0,   0,   0,   0,   0,   0,   0,   0,        /* F1 .. F10 */
        0,                                                       /* numlock */
        0,                                                       /* scroll lock */
        0,                                                       /* home */
        0,                                                       /* up arrow */
        0,                                                       /* page up */
        '-',                                                     /**/
        0,                                                       /* left arrow */
        0,                                                       /**/
        0,                                                       /* right arrow */
        '+',                                                     /**/
        0,                                                       /* end key */
        0,                                                       /* down arrow */
        0,                                                       /* page down */
        0,                                                       /* insert */
        0,                                                       /* delete */
        0,    0,    0,                                           /**/
        0,                                                       /* f11 key */
        0,                                                       /* f12 key */
        0, /* all other keys are undefined */
      },
};
