
#ifndef LAYOUTS_ES_H_
#define LAYOUTS_ES_H_ 1

#include <kernel/keyboard.h> /* Layout */

/** @todo Fix some keys */

const Layout es_layout = {
    .def =
      (unsigned char[]){
        0,    /**/
        0x1b, /* esc */
        '1',  '2',  '3',  '4', '5', '6', '7', '8', '9', '0', '\'', '¡', /**/
        '\b', '\t',                                                     /**/
        'q',  'w',  'e',  'r', 't', 'y', 'u', 'i', 'o', 'p',            /**/
        '`',  '+',  '\n',                                               /**/
        0, /* left ctrl */
        'a',  's',  'd',  'f', 'g', 'h', 'j', 'k', 'l', 'ñ', '´',  'º', /**/
        0, /* left shift */
        'ç',  'z',  'x',  'c', 'v', 'b', 'n', 'm', ',', '.', '-', /**/
        0,                                                 /* right shift */
        '*',                                               /**/
        0,                                                 /* alt */
        ' ',                                               /* space */
        0,                                                 /* capslock */
        0,    0,    0,    0,   0,   0,   0,   0,   0,   0, /* F1 .. F10 */
        0,                                                 /* numlock */
        0,                                                 /* scroll Lock */
        0,                                                 /* home */
        0,                                                 /* up arrow */
        0,                                                 /* page up */
        '\'',                                              /**/
        0,                                                 /* left arrow */
        0,                                                 /**/
        0,                                                 /* right arrow */
        '¡',                                               /**/
        0,                                                 /* end */
        0,                                                 /* down arrow */
        0,                                                 /* page down */
        0,                                                 /* insert */
        0,                                                 /* delete */
        0,    0,    '<',                                   /**/
        0,                                                 /* f11 key */
        0,                                                 /* f12 key */
        0, /* all other keys are undefined */
      },
    .shift =
      (unsigned char[]){
        0,                                                             /**/
        0x1b,                                                          /* esc */
        '!',  '\"', '#', '$', '%', '&', '/', '(', ')', '=', '?',  '¿', /**/
        '\b', '\t',                                                    /**/
        'Q',  'W',  'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '^',  '*',
        '\n', /**/
        0,    /* left ctrl */
        'A',  'S',  'D', 'F', 'G', 'H', 'J', 'K', 'L', 'Ñ', '\"', 'ª', /**/
        0, /* left shift */
        'Ç',  'Z',  'X', 'C', 'V', 'B', 'N', 'M', ';', ':', '_', /**/
        0,                                                /* right shift */
        '(',                                              /**/
        0,                                                /* alt */
        ' ',                                              /* space */
        0,                                                /* capslock */
        0,    0,    0,   0,   0,   0,   0,   0,   0,   0, /* F1 .. F10 */
        0,                                                /* numlock */
        0,                                                /* scroll lock */
        0,                                                /* home */
        0,                                                /* up arrow */
        0,                                                /* page up */
        '?',                                              /**/
        0,                                                /* left arrow */
        0,                                                /**/
        0,                                                /* right arrow */
        '¿',                                              /**/
        0,                                                /* end */
        0,                                                /* down arrow */
        0,                                                /* page down */
        0,                                                /* insert */
        0,                                                /* delete */
        0,    0,    '>',                                  /**/
        0,                                                /* f11 key */
        0,                                                /* f12 key */
        0, /* all other keys are undefined */
      },
    .special =
      (uint16_t[]){
        [KB_SPECIAL_IDX_ESC]         = 0x1,  /**/
        [KB_SPECIAL_IDX_LCTRL]       = 0x1D, /**/
        [KB_SPECIAL_IDX_LSHIFT]      = 0x2A, /**/
        [KB_SPECIAL_IDX_RSHIFT]      = 0x36, /**/
        [KB_SPECIAL_IDX_LALT]        = 0x38, /**/
        [KB_SPECIAL_IDX_CAPSLOCK]    = 0x3A, /**/
        [KB_SPECIAL_IDX_F1]          = 0x3B, /**/
        [KB_SPECIAL_IDX_F2]          = 0x3C, /**/
        [KB_SPECIAL_IDX_F3]          = 0x3D, /**/
        [KB_SPECIAL_IDX_F4]          = 0x3E, /**/
        [KB_SPECIAL_IDX_F5]          = 0x3F, /**/
        [KB_SPECIAL_IDX_F6]          = 0x40, /**/
        [KB_SPECIAL_IDX_F7]          = 0x41, /**/
        [KB_SPECIAL_IDX_F8]          = 0x42, /**/
        [KB_SPECIAL_IDX_F9]          = 0x43, /**/
        [KB_SPECIAL_IDX_F10]         = 0x44, /**/
        [KB_SPECIAL_IDX_NUMLOCK]     = 0x45, /**/
        [KB_SPECIAL_IDX_SCROLL_LOCK] = 0x46, /**/
        [KB_SPECIAL_IDX_HOME]        = 0x47, /**/
        [KB_SPECIAL_IDX_ARROW_UP]    = 0x48, /**/
        [KB_SPECIAL_IDX_PAGE_UP]     = 0x49, /**/
        [KB_SPECIAL_IDX_ARROW_LEFT]  = 0x4B, /**/
        [KB_SPECIAL_IDX_ARROW_RIGHT] = 0x4D, /**/
        [KB_SPECIAL_IDX_END]         = 0x4F, /**/
        [KB_SPECIAL_IDX_ARROW_DOWN]  = 0x50, /**/
        [KB_SPECIAL_IDX_PAGE_DOWN]   = 0x51, /**/
        [KB_SPECIAL_IDX_INSERT]      = 0x52, /**/
        [KB_SPECIAL_IDX_DELETE]      = 0x53, /**/
        [KB_SPECIAL_IDX_F11]         = 0x57, /**/
        [KB_SPECIAL_IDX_F12]         = 0x58, /**/
      },
};

#endif /* LAYOUTS_ES_H_ */
