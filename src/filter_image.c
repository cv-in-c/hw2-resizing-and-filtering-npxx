#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    // TODO
    float img_sum = 0;
    for (int i = 0; i < im.w * im.h * im.c; i++)
    {
        img_sum += im.data[i];
    }

    if (img_sum == 0)
    {
        for (int i = 0; i < im.w * im.h * im.c; i++)
        {
            im.data[i] = 1.0 / (im.w * im.h * im.c);
        }
    }

    else
    {
        for (int i = 0; i < im.w * im.h * im.c; i++)
        {
            im.data[i] /= img_sum;
        }
    }
}

image make_box_filter(int w)
{
    // TODO
    image box_filter = make_image(w, w, 1); // square box filter of w x w, with one channel
    for (int i = 0; i < w * w; i++)
    {
        box_filter.data[i] = 1;
    }

    l1_normalize(box_filter); // normalised
    return box_filter;
}

image convolve_image(image im, image filter, int preserve)
{
    // TODO

    // This is

    // Checking number of channels
    assert(im.c == filter.c || filter.c == 1);

    image convolved = make_image(im.w, im.h, im.c);
    for (int c = 0; c < im.c; c++)
    {
        for (int y = 0; y < im.h; y++)
        {
            for (int x = 0; x < im.w; x++)
            {
                float fil = 0;
                if (filter.c != 1)
                    fil = c;

                float sum = 0;
                for (int fy = 0; fy < filter.h; fy++)
                {
                    for (int fx = 0; fx < filter.w; fx++)
                    {
                        int im_x = x + fx - filter.w / 2;
                        int im_y = y + fy - filter.h / 2;
                        float im_val = get_pixel(im, im_x, im_y, c);
                        float filter_val = get_pixel(filter, fx, fy, fil);
                        sum += im_val * filter_val;
                    }
                }
                set_pixel(convolved, x, y, c, sum);
            }
        }
    }

    // Number of channels not preserved
    if (preserve == 0)
    {
        image convolved_1 = make_image(im.w, im.h, 1);
        for (int y = 0; y < im.h; y++)
        {
            for (int x = 0; x < im.w; x++)
            {
                float sum = 0;
                for (int c = 0; c < im.c; c++)
                {
                    sum += get_pixel(convolved, x, y, c);
                }
                set_pixel(convolved_1, x, y, 0, sum);
            }
        }
        return convolved_1;
    }

    return convolved;
}

image make_highpass_filter()
{
    // TODO
    // Matrix
    // 0 -1 0
    // -1  4 -1
    // 0 -1 0

    image highpass_filter = make_box_filter(3);
    highpass_filter.data[0] = highpass_filter.data[2] = highpass_filter.data[6] = highpass_filter.data[8] = 0;

    highpass_filter.data[1] = highpass_filter.data[3] = highpass_filter.data[5] = highpass_filter.data[7] = -1;

    highpass_filter.data[4] = 4;

    return highpass_filter;
}

image make_cannyedge_filter()
{
    // TODO
    // Matrix
    // -1 -1 -1
    // -1  8 -1
    // -1 -1 -1

    image cannyedge_filter = make_box_filter(3);
    cannyedge_filter.data[0] = cannyedge_filter.data[2] = cannyedge_filter.data[6] = cannyedge_filter.data[8] = cannyedge_filter.data[1] = cannyedge_filter.data[3] = cannyedge_filter.data[5] = cannyedge_filter.data[7] = -1;

    cannyedge_filter.data[4] = 8;

    return cannyedge_filter;
}

image make_sharpen_filter()
{
    // TODO
    // Matrix
    // 0 -1 0
    // -1  5 -1
    // 0 -1 0

    image sharpen_filter = make_box_filter(3);
    sharpen_filter.data[0] = sharpen_filter.data[2] = sharpen_filter.data[6] = sharpen_filter.data[8] = 0;

    sharpen_filter.data[1] = sharpen_filter.data[3] = sharpen_filter.data[5] = sharpen_filter.data[7] = -1;

    sharpen_filter.data[4] = 5;

    return sharpen_filter;
}

image make_emboss_filter()
{
    // TODO
    // Matrix
    // -2 -1 0
    // -1 1 1
    // 0 1 2

    image emboss_filter = make_box_filter(3);

    emboss_filter.data[0] = -2;
    emboss_filter.data[4] = 1;
    emboss_filter.data[8] = 2;

    emboss_filter.data[2] = emboss_filter.data[6] = 0;
    emboss_filter.data[1] = emboss_filter.data[3] = -1;
    emboss_filter.data[5] = emboss_filter.data[7] = 1;

    return emboss_filter;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: Sharpen & Emboss (since we want all the three channels)

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: Clamping for the prevention of overflowing images

image make_gaussian_filter(float sigma)
{
    // TODO
    int ksize = (int)(2 * ceilf(3 * sigma)) + 1;
    image gaussian_filter = make_box_filter(ksize);
    int kshalf = (int)ksize / 2;

    for (int y = 0; y < ksize; y++)
        for (int x = 0; x < ksize; x++)
        {
            float coeff = 1 / (sigma * sigma * TWOPI);
            float power = -((x - kshalf) * (x - kshalf) + (y - kshalf) * (y - kshalf)) / (sigma * sigma * 2);
            gaussian_filter.data[x + y * ksize] = coeff * exp(power);
        }

    l1_normalize(gaussian_filter); //"If only we had a function for that" :))
    return gaussian_filter;
}

image add_image(image a, image b)
{
    // TODO
    assert(a.w == b.w && a.h == b.h && a.c == b.c);
    image addn = make_image(a.w, a.h, a.c);
    for (int i = 0; i < a.w * a.h * a.c; ++i)
        addn.data[i] = a.data[i] + b.data[i];

    return addn;
}

image sub_image(image a, image b)
{
    // TODO
    assert(a.w == b.w && a.h == b.h && a.c == b.c);
    image subn = make_image(a.w, a.h, a.c);
    for (int i = 0; i < a.w * a.h * a.c; ++i)
        subn.data[i] = a.data[i] - b.data[i];

    return subn;
}

image make_gx_filter()
{
    // TODO
    return make_image(1, 1, 1);
}

image make_gy_filter()
{
    // TODO
    return make_image(1, 1, 1);
}

void feature_normalize(image im)
{
    // TODO
}

image *sobel_image(image im)
{
    // TODO
    return calloc(2, sizeof(image));
}

image colorize_sobel(image im)
{
    // TODO
    return make_image(1, 1, 1);
}
