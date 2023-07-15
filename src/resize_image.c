#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    // According to given formula, f(x,y,z) = Im(round(x), round(y), z)
    return get_pixel(im, round(x), round(y), c);
}

image nn_resize(image im, int w, int h)
{
    // TODO Fill in (also fix that first line)
    image resized = make_image(w, h, im.c);
    float x_scale = (float)im.w / w;
    float y_scale = (float)im.h / h;
    float x_final, y_final;
    for (int c = 0; c < im.c; c++)
    {
        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++)
            {
                x_final = x * x_scale;
                y_final = y * y_scale;
                set_pixel(resized, x, y, c, nn_interpolate(im, x_final, y_final, c));
            }
        }
    }
    return resized;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    // TODO

    // We want four pixels (top left, top right, bottom left, bottom right, say V1, V2, V3, V4) to interpolate between.

    int x_1 = floorf(x);
    int x_2 = ceilf(x);
    int y_1 = floorf(y);
    int y_2 = ceilf(y);

    float V1 = get_pixel(im, x_1, y_1, c);
    float V2 = get_pixel(im, x_2, y_1, c);
    float V3 = get_pixel(im, x_1, y_2, c);
    float V4 = get_pixel(im, x_2, y_2, c);

    // Finding areas of rectangles (as in slides)

    float A1 = (x_2 - x) * (y_2 - y);
    float A2 = (x - x_1) * (y_2 - y);
    float A3 = (x_2 - x) * (y - y_1);
    float A4 = (x - x_1) * (y - y_1);

    // Finding weighted average

    float q = (A1 * V1) + (A2 * V2) + (A3 * V3) + (A4 * V4);

    return q;
}

image bilinear_resize(image im, int w, int h)
{
    // TODO
    image bi_resized = make_image(w, h, im.c);
    float x_scale = (float)im.w / w;
    float y_scale = (float)im.h / h;
    float x_final, y_final;
    for (int c = 0; c < im.c; c++)
    {
        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++)
            {
                x_final = x * x_scale;
                y_final = y * y_scale;
                set_pixel(bi_resized, x, y, c, bilinear_interpolate(im, x_final, y_final, c));
            }
        }
    }

    return bi_resized;
}
