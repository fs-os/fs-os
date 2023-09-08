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
#include <stdlib.h>
#include <math.h> /* fabs, fmod */

#include <kernel/keyboard.h>
#include <kernel/framebuffer.h>

/* Keys */
#define KEY_ZOOM_IN  '.'
#define KEY_ZOOM_OUT ','
#define KEY_UP       'k'
#define KEY_DOWN     'j'
#define KEY_LEFT     'h'
#define KEY_RIGHT    'l'
#define KEY_ITER_INC 'i'
#define KEY_ITER_DEC 'u'
#define KEY_RESET    'r'
#define KEY_QUIT     'q'

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

/* RGB color used for drawing the interior of the mandelbrot set */
#define INSIDE_COL 0x000000

static uint32_t hue2rgb(float h);

/**
 * @todo Use windows once it's added.
 * @todo Being able to... exit
 */
int main_mandelbrot() {
    bool was_kb_echo = kb_getecho();
    kb_noecho();

    bool was_kb_raw = kb_getraw();
    kb_raw();

    /* Framebuffer. Each 32 bit entry is a color */
    volatile uint32_t* fb = fb_get_ptr();

    const uint32_t w = fb_get_width();
    const uint32_t h = fb_get_height();

    /* Can change with KEY_ITER_INC and KEY_ITER_DEC */
    uint32_t max_iter = DEFAULT_MAX_ITER;

    /* Will become smaller when zooming */
    double zoom     = DEFAULT_ZOOM;
    double x_offset = DEFAULT_X_OFF;
    double y_offset = DEFAULT_X_OFF;

    /* Will be scaled when zooming, so we don't move too much */
    double move_step = DEFAULT_MOVE_STEP;

    /* Calculate some values here for performance */
    const double scaled_h = h / 2.0;
    const double scaled_w = w / 3.0;

    bool main_loop = true;
    while (main_loop) {
        /* Mandelbrot */
        for (uint32_t y_px = 0; y_px < h; y_px++) {
            /* Real Y is the mandelbrot center vertically. We subtract 1.0 (half
             * the height) to center it vertically. */
            double real_y = (y_px / scaled_h) - 1.0;
            real_y *= zoom;
            real_y += y_offset;

            for (uint32_t x_px = 0; x_px < w; x_px++) {
                /* Real X is the mandelbrot center horizontally. We subtract 2.0
                 * (half the width) to center it horizontally. */
                /* TODO: Not zooming perfectly centered */
                double real_x = (x_px / scaled_w) - 2.0;
                real_x *= zoom;
                real_x += x_offset;

                /* These 2 values will be increased each iteration bellow */
                double x = real_x;
                double y = real_y;

                bool inside_set = true;

                /* In each iteration, we will check if we are inside the
                 * mandebrot set. An interesting property of the mandelbrot set
                 * is that the more iterations an outside value takes, the
                 * closer to the set is. We can use this to change colors.
                 * Ouside of the loop. */
                uint32_t iter;
                for (iter = 0; iter < max_iter; iter++) {
                    /* Calulate squares once */
                    double sqr_x = x * x;
                    double sqr_y = y * y;

                    /* Absolute value of a complex number is the distance from
                     * origin: sqrt(x^2 + y^2) > 2 */
                    if ((sqr_x + sqr_y) > 2 * 2) {
                        inside_set = false;
                        break;
                    }

                    /* This part is explained in the povusers.org link on
                     * credits */
                    y = (2.0 * x * y) + real_y;
                    x = (sqr_x - sqr_y) + real_x;
                }

                /* If it's inside the set, draw fixed color */
                if (inside_set) {
                    fb[y_px * w + x_px] = INSIDE_COL;
                    continue;
                }

                /* Get 0..360 hue for color based on iter..max_iter */
                int scaled_hue      = iter * MAX_H / max_iter;
                fb[y_px * w + x_px] = hue2rgb(scaled_hue);
            }
        }

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
            case KEY_QUIT:
                /* TODO */
                main_loop = false;
                break;
            default:
                break;
        }
    }

    if (was_kb_echo)
        kb_echo();

    if (!was_kb_raw)
        kb_noraw();

    return 0;
}

static uint32_t hue2rgb(float h) {
    float prime = fmod(h / 60.f, 6);
    float x     = 1 - fabs(fmod(prime, 2) - 1);

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
