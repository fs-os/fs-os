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

#include <kernel/framebuffer.h>

/* Max HSV values (Used for scaling) */
#define MAX_H 360
#define MAX_S 1.0
#define MAX_V 1.0

/* Default HSV values */
#define COL_H 360
#define COL_S 1.0
#define COL_V 1.0

/* Smaller -> More zoom (1.0 is defaul) */
#define ZOOM 1.0

/* Offsets for moving in the set. */
#define X_OFFSET 0.0 /* -2.0..+2.0 (Left..Right) */
#define Y_OFFSET 0.0 /* -1.0..+1.0 (Up..Down) */

/* RGB color used for drawing the interior of the mandelbrot set */
#define INSIDE_COL 0x000000

static void hsv2rgb(float* r, float* g, float* b, float h, float s, float v);

/** @todo Use windows once it's added */
int main_mandelbrot(int argc, char** argv) {
    if (argc != 2) {
        printf("Wrong number of arguments.\n"
               "Usage: %s <iterations>\n",
               argv[0]);
        return 1;
    }

    const uint32_t w   = fb_get_width();
    const uint32_t h   = fb_get_height();
    const int max_iter = atoi(argv[1]);

    if (max_iter < 1 || max_iter > 255) {
        printf("Wrong iteration argument (1..255).\n"
               "Usage: %s <iterations>\n",
               argv[0]);
        return 1;
    }

    /* Framebuffer. Each 32 bit entry is a color */
    volatile uint32_t* fb = fb_get_ptr();

    /* Calculate some values here for performance */
    const double scaled_h = h / 2.0;
    const double scaled_w = w / 3.0;

    /** @todo Interactive loop */

    /* Mandelbrot */
    for (uint32_t y_px = 0; y_px < h; y_px++) {
        /* Real Y is the mandelbrot center vertically. We subtract 1.0 (half the
         * height) to center it vertically. */
        double real_y = (y_px / scaled_h) - 1.0;
        real_y *= ZOOM;
        real_y += Y_OFFSET;

        for (uint32_t x_px = 0; x_px < w; x_px++) {
            /* Real X is the mandelbrot center horizontally. We subtract 2.0
             * (half the width) to center it horizontally. */
            double real_x = (x_px / scaled_w) - 2.0;
            real_x *= ZOOM;
            real_x += X_OFFSET;

            /* These 2 values will be increased each iteration bellow */
            double x = real_x;
            double y = real_y;

            bool inside_set = true;

            /* In each iteration, we will check if we are inside the mandebrot
             * set. An interesting property of the mandelbrot set is that the
             * more iterations an outside value takes, the closer to the set is.
             * We can use this to change colors. Ouside of the loop. */
            int iter;
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

                /* This part is explained in the povusers.org link on credits */
                y = (2.0 * x * y) + real_y;
                x = (sqr_x - sqr_y) + real_x;
            }

            /* If it's inside the set, draw fixed color */
            if (inside_set) {
                fb[y_px * w + x_px] = INSIDE_COL;
                continue;
            }

            /* Get 0..360 hue for color based on iter..max_iter
             *
             * NOTE: Make sure you change the variable type to double if you
             * want to scale the Saturation or Value instead (to not truncate to
             * zero) */
            int scaled_hue = iter * COL_H / max_iter;

            float fl_r, fl_g, fl_b;
            hsv2rgb(&fl_r, &fl_g, &fl_b, scaled_hue, COL_S, COL_V);

            /* NOTE: Try to replace some of these parameters with zeros or mess
             * with the scaling and see what happens :) */
            int r = fl_r * 255.f;
            int g = fl_g * 255.f;
            int b = fl_b * 255.f;

            fb[y_px * w + x_px] = rgb2col(r, g, b);
        }
    }

    /* FIXME */
    for (;;)
        ;

    return 0;
}

static void hsv2rgb(float* r, float* g, float* b, float h, float s, float v) {
    float chroma = v * s; /* Chroma */
    float prime  = fmod(h / 60.f, 6);
    float x      = chroma * (1 - fabs(fmod(prime, 2) - 1));
    float m      = v - chroma;

    *r = 0.f;
    *g = 0.f;
    *b = 0.f;

    if (prime >= 0 && prime < 1) {
        *r = chroma;
        *g = x;
    } else if (prime >= 1 && prime < 2) {
        *r = x;
        *g = chroma;
    } else if (prime >= 2 && prime < 3) {
        *g = chroma;
        *b = x;
    } else if (prime >= 3 && prime < 4) {
        *g = x;
        *b = chroma;
    } else if (prime >= 4 && prime < 5) {
        *r = x;
        *b = chroma;
    } else if (prime >= 5 && prime < 6) {
        *r = chroma;
        *b = x;
    }

    *r += m;
    *g += m;
    *b += m;
}
