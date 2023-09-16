/*
 * Forked and interactive version of:
 * https://github.com/8dcc/c-stuff/blob/main/Misc/ppm-mandelbrot.c
 *
 * On mandelbrot, see:
 * - https://en.wikipedia.org/wiki/Mandelbrot_set
 * - http://warp.povusers.org/Mandelbrot/
 *   https://web.archive.org/web/20230324155752/http://warp.povusers.org/Mandelbrot/
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h> /* fabs, fmod */

#include <kernel/font.h>
#include <kernel/keyboard.h>
#include <kernel/framebuffer.h>

/* If defined, the program will use an intermediate buffer to avoid tearing */
#define DOUBLE_BUFFERING

/* Keys */
#define KEY_ZOOM_IN     '.'
#define KEY_ZOOM_OUT    ','
#define KEY_UP          'k'
#define KEY_DOWN        'j'
#define KEY_LEFT        'h'
#define KEY_RIGHT       'l'
#define KEY_ITER_INC    'i'
#define KEY_ITER_DEC    'u'
#define KEY_RESET       'r'
#define KEY_TOGGLE_AXIS 'g'
#define KEY_QUIT        'q'

/* Max hue value (Used for scaling) */
#define MAX_H 360

/* Smaller -> More zoom (1.0 is defaul) */
#define DEFAULT_ZOOM 1.0

/* Offsets for moving in the set. */
#define DEFAULT_X_OFF 0.0 /* -2.0..+2.0 (Left..Right) */
#define DEFAULT_Y_OFF 0.0 /* -1.0..+1.0 (Up..Down) */

/* Higher means more detail */
#define DEFAULT_MAX_ITER 100

/* Default steps for KEY_* controls */
#define ZOOM_STEP         0.5
#define DEFAULT_MOVE_STEP 0.1
#define ITER_STEP         10

/* Fixed RGB color used for drawing the pixels inside the mandelbrot set */
#define INSIDE_COL 0x000000

/* Color used to draw the X and Y axis, if enabled */
#define AXIS_COL 0xDDDDDD

static uint32_t hue2rgb(float h);

/**
 * @todo Use windows once it's added.
 * @todo Being able to... exit
 */
int main_mandelbrot(int argc, char** argv) {
    if (argc > 1 && (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))) {
        printf("Keys:\n"
               "    %c - Zoom in\n"
               "    %c - Zoom out\n"
               "    %c - Move up\n"
               "    %c - Move down\n"
               "    %c - Move left\n"
               "    %c - Move right\n"
               "    %c - Increase iterations (precision)\n"
               "    %c - Decrease iterations (precision)\n"
               "    %c - Reset (position, zoom, precision)\n"
               "    %c - Toggle X and Y axis\n"
               "    %c - Quit (WIP)\n",
               KEY_ZOOM_IN, KEY_ZOOM_OUT, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
               KEY_ITER_INC, KEY_ITER_DEC, KEY_RESET, KEY_TOGGLE_AXIS,
               KEY_QUIT);
        return 1;
    }

    bool was_kb_echo = kb_getecho();
    kb_noecho();

    bool was_kb_raw = kb_getraw();
    kb_raw();

    /* Framebuffer. Each 32 bit entry is a color. We will draw the mandelbrot to
     * the intermediate `pixels` ptr, which will be overwritten if
     * DOUBLE_BUFFERING is defined. */
    volatile uint32_t* fb     = fb_get_ptr();
    volatile uint32_t* pixels = fb;

    const uint32_t w = fb_get_width();
    const uint32_t h = fb_get_height();

#ifdef DOUBLE_BUFFERING
    /* Use double buffering */
    uint32_t* back_buff = calloc(w * h, sizeof(uint32_t));
    pixels              = back_buff;
#endif

    /* Draw X and Y axis. Toggled with KEY_TOGGLE_AXIS */
    bool draw_axis = false;

    /* Can change with KEY_ITER_INC and KEY_ITER_DEC */
    uint32_t max_iter = DEFAULT_MAX_ITER;

    /* Will become smaller when zooming */
    double zoom     = DEFAULT_ZOOM;
    double x_offset = DEFAULT_X_OFF;
    double y_offset = DEFAULT_X_OFF;

    /* Will be scaled when zooming, so we don't move too much */
    double move_step = DEFAULT_MOVE_STEP;

    /* Calculate some values here for performance */
    const double min_real_x = -2.0;
    const double max_real_x = 1.0;
    const double min_real_y = -1.0;
    const double max_real_y = 1.0;

    const double factor_x = (max_real_x - min_real_x) / (w - 1);
    const double factor_y = (max_real_y - min_real_y) / (h - 1);

    bool main_loop = true;
    while (main_loop) {
        /* Increased every drawn pixel. Faster than [y * w + x] */
        uint32_t fb_idx = 0;

        /* Mandelbrot */
        for (uint32_t y_px = 0; y_px < h; y_px++) {
            /* Real Y is the mandelbrot center vertically. We subtract 1.0 (half
             * the height) to center it vertically. */
            double real_y = min_real_y + y_px * factor_y;

            /* Unlike real_x, because real_y values are -1..+1 (min/max_real_y),
             * we can just zoom at +0 (the center) */
            real_y *= zoom;
            real_y += y_offset;

            for (uint32_t x_px = 0; x_px < w; x_px++) {
                /* Real X is the mandelbrot center horizontally. We subtract 2.0
                 * (half the width) to center it horizontally. */
                double real_x = min_real_x + x_px * factor_x;

                /* Because real_x values are -2..+1 (min/max_real_x), we zoom at
                 * -0.5 (the center) and then we restore. */
                real_x += 0.5;
                real_x *= zoom;
                real_x -= 0.5;
                real_x += x_offset;

                /* These 2 values will be increased each iteration bellow */
                double x = real_x;
                double y = real_y;

                bool inside_set = true;

                /* In each iteration, we will check if we are inside the
                 * mandebrot set. An interesting property of the mandelbrot set
                 * is that the more iterations an outside value takes, the
                 * closer to the set is. We can use this to change colors. */
                for (uint32_t iter = 0; iter < max_iter; iter++) {
                    /* Calulate squares once */
                    double sqr_x = x * x;
                    double sqr_y = y * y;

                    /* Absolute value of a complex number is the distance from
                     * origin: sqrt(x^2 + y^2) > 2 */
                    if (sqr_x + sqr_y > 2 * 2) {
                        /* Scale 0..360 HUE based on iter..max_iter ratio */
                        int scaled_hue = iter * MAX_H / max_iter;
                        pixels[fb_idx] = hue2rgb(scaled_hue);

                        inside_set = false;
                        break;
                    }

                    /* This part is explained in the povusers link on credits */
                    y = (2.0 * x * y) + real_y;
                    x = (sqr_x - sqr_y) + real_x;
                }

                /* We passed all iterations, we are inside the set. */
                if (inside_set)
                    pixels[fb_idx] = INSIDE_COL;

                fb_idx++;
            }
        }

        if (draw_axis) {
            const uint32_t mid_x = w / 2;
            const uint32_t mid_y = h / 2;

            for (uint32_t y_px = 0; y_px < h; y_px++)
                pixels[y_px * w + mid_x] = AXIS_COL;

            for (uint32_t x_px = 0; x_px < w; x_px++)
                pixels[mid_y * w + x_px] = AXIS_COL;
        }

#ifdef DOUBLE_BUFFERING
        /* Swap the buffers */
        for (uint32_t i = 0; i < w * h; i++)
            fb[i] = pixels[i];
#endif

        static char iter_str[] = "Iters: 9999";
        itoa(&iter_str[7], max_iter);

        static color_pair cols = { 0xFFFFFF, 0x000000 };
        fb_drawtext(5, 5, cols, &main_font, iter_str);

        /* Get user input */
        switch (getchar()) {
            case KEY_ZOOM_IN:
                zoom *= ZOOM_STEP;
                move_step *= ZOOM_STEP;
                break;
            case KEY_ZOOM_OUT:
                zoom /= ZOOM_STEP;
                move_step /= ZOOM_STEP;
                break;
            case KEY_UP:
                if (y_offset > -1.1)
                    y_offset -= move_step;
                break;
            case KEY_DOWN:
                if (y_offset < 1.1)
                    y_offset += move_step;
                break;
            case KEY_LEFT:
                if (x_offset > -2.1)
                    x_offset -= move_step;
                break;
            case KEY_RIGHT:
                if (x_offset < 2.1)
                    x_offset += move_step;
                break;
            case KEY_ITER_INC:
                max_iter += ITER_STEP;
                break;
            case KEY_ITER_DEC:
                if (max_iter > ITER_STEP)
                    max_iter -= ITER_STEP;
                break;
            case KEY_RESET:
                zoom      = DEFAULT_ZOOM;
                move_step = DEFAULT_MOVE_STEP;
                x_offset  = DEFAULT_X_OFF;
                y_offset  = DEFAULT_X_OFF;
                max_iter  = DEFAULT_MAX_ITER;
                break;
            case KEY_TOGGLE_AXIS:
                draw_axis = !draw_axis;
                break;
            case KEY_QUIT:
                /* TODO */
                main_loop = false;
                break;
            default:
                break;
        }
    }

#ifdef DOUBLE_BUFFERING
    free(back_buff);
#endif

    if (was_kb_echo)
        kb_echo();

    if (!was_kb_raw)
        kb_noraw();

    return 0;
}

static uint32_t hue2rgb(float h) {
    float prime = fmodf(h / 60.f, 6);
    float x     = 1 - fabsf(fmodf(prime, 2) - 1);

    uint32_t ret = 0x000000;

    if (prime >= 0 && prime < 1) {
        ret |= 0xFF0000;                 /* r = 255 */
        ret |= (uint8_t)(x * 0xFF) << 8; /* g = x */
    } else if (prime < 2) {
        ret |= 0x00FF00;                  /* g = 255 */
        ret |= (uint8_t)(x * 0xFF) << 16; /* r = x */
    } else if (prime < 3) {
        ret |= 0x00FF00;                 /* g = 255 */
        ret |= (uint8_t)(x * 0xFF) << 0; /* b = x */
    } else if (prime < 4) {
        ret |= 0x0000FF;                 /* b = 255 */
        ret |= (uint8_t)(x * 0xFF) << 8; /* g = x */
    } else if (prime < 5) {
        ret |= 0x0000FF;                  /* b = 255 */
        ret |= (uint8_t)(x * 0xFF) << 16; /* r = x */
    } else if (prime < 6) {
        ret |= 0xFF0000;                 /* r = 255 */
        ret |= (uint8_t)(x * 0xFF) << 0; /* b = x */
    }

    return ret;
}
