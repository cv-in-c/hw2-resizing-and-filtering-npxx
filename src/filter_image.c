#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    float img_sum = 0;

    for (int i = 0; i < im.w * im.h * im.c; i++)
    {
        img_sum += im.data[i];
    }

    for (int i = 0; i < im.w * im.h * im.c; i++)
    {
        if (img_sum == 0)
            im.data[i] = 1.0 / (im.w * im.h * im.c);
        else
            im.data[i] /= img_sum;
    }
}

image make_box_filter(int w)
{
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
    // Checking number of channels
    assert(im.c == filter.c || filter.c == 1);

    image convolved = make_image(im.w, im.h, im.c);

    for (int c = 0; c < im.c; c++)
    {
        for (int y = 0; y < im.h; y++)
        {
            for (int x = 0; x < im.w; x++)
            {
                float fil = (filter.c == 1) ? 0 : c;
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
// Answer: Sharpen & Emboss (since we want all the three channels), whereas filters like Grayscale may not preserve the number of channels

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: Clamping for the prevention of overflowing images

image make_gaussian_filter(float sigma)
{
    int ksize = (int)(2 * ceilf(3 * sigma)) + 1; // to get an odd-number sized kernel
    int kshalf = (int)ksize / 2;

    image gaussian_filter = make_box_filter(ksize);

    for (int y = 0; y < ksize; y++)
        for (int x = 0; x < ksize; x++)
        {
            float coeff = 1 / (sigma * sigma * TWOPI);
            float power = -((x - kshalf) * (x - kshalf) + (y - kshalf) * (y - kshalf)) / (sigma * sigma * 2);
            gaussian_filter.data[x + y * ksize] = coeff * exp(power);
        }

    l1_normalize(gaussian_filter); // "If only we had a function for that" :))

    return gaussian_filter;
}

image add_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c); // Sanity check

    image addn = make_image(a.w, a.h, a.c);

    for (int i = 0; i < a.w * a.h * a.c; ++i)
        addn.data[i] = a.data[i] + b.data[i];

    return addn;
}

image sub_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c);

    image subn = make_image(a.w, a.h, a.c);

    for (int i = 0; i < a.w * a.h * a.c; ++i)
        subn.data[i] = a.data[i] - b.data[i];

    return subn;
}

image make_gx_filter()
{
    // for horizontal edge detection
    // Matrix
    // -1 0 1
    // -2 0 2
    // -1 0 1

    image gx = make_box_filter(3);

    gx.data[0] = -1;
    gx.data[1] = 0;
    gx.data[2] = 1;
    gx.data[3] = -2;
    gx.data[4] = 0;
    gx.data[5] = 2;
    gx.data[6] = -1;
    gx.data[7] = 0;
    gx.data[8] = 1;

    return gx;
}

image make_gy_filter()
{
    // for vertical edge detection
    // Matrix
    // -1 -2 1
    //  0 0 0
    // 1 2 1

    image gy = make_box_filter(3);

    gy.data[0] = -1;
    gy.data[1] = -2;
    gy.data[2] = -1;
    gy.data[3] = 0;
    gy.data[4] = 0;
    gy.data[5] = 0;
    gy.data[6] = 1;
    gy.data[7] = 2;
    gy.data[8] = 1;

    return gy;
}

void feature_normalize(image im)
{
    float max = -1.0;
    float min = INFINITY;

    // Getting the max and min pixel values from our image
    for (int i = 0; i < im.w; i++)
    {
        for (int y = 0; y < im.h; y++)
        {
            for (int c = 0; c < im.c; c++)
            {
                float testv = im.data[i + y * im.w + c * im.w * im.h];
                if (testv > max)
                    max = testv;
                if (testv < min)
                    min = testv;
            }
        }
    }

    // Rescaling
    float range = max - min;
    for (int x = 0; x < im.w; x++)
    {
        for (int y = 0; y < im.h; y++)
        {
            for (int c = 0; c < im.c; c++)
            {
                if (range != 0)
                    set_pixel(im, x, y, c, (get_pixel(im, x, y, c) - min) / range);
                else
                    set_pixel(im, x, y, c, 0);
            }
        }
    }
}

image *sobel_image(image im)
{
    image *sobel = calloc(2, sizeof(image));

    image fgrad_x = make_gx_filter();
    image fgrad_y = make_gy_filter();

    // Now that we have the gradient filters, fgrad_x and fgrad_y,
    // We can construct the approximation of derivates in either direction
    // Gx = fgrad_x (*) im [convolution]
    // Gy = fgrad_y (*) im

    image gx = convolve_image(im, fgrad_x, 0);
    image gy = convolve_image(im, fgrad_y, 0);

    sobel[0] = make_image(im.w, im.h, 1);
    sobel[1] = make_image(im.w, im.h, 1);

    // Now, gradient magnitude and direction
    // G = sqrt(Gx^2 + Gy^2)
    // \Theta = atan2(Gy, Gx)

    for (int x = 0; x < im.w; x++)
    {
        for (int y = 0; y < im.h; y++)
        {
            float Gx = get_pixel(gx, x, y, 0);
            float Gy = get_pixel(gy, x, y, 0);

            float G_mag = sqrt(Gx * Gx + Gy * Gy);
            float G_theta = atan2(Gy, Gx);

            set_pixel(sobel[0], x, y, 0, G_mag);
            set_pixel(sobel[1], x, y, 0, G_theta);
        }
    }

    return sobel;
}

image colorize_sobel(image im)
{
    image color_sobel = make_image(im.w, im.h, im.c);

    image fgrad_x = make_gx_filter();
    image fgrad_y = make_gy_filter();

    // We are preserving this time
    image gx = convolve_image(im, fgrad_x, 1);
    image gy = convolve_image(im, fgrad_y, 1);

    // Can colourize in any way
    for (int x = 0; x < im.w; x++)
    {
        for (int y = 0; y < im.h; y++)
        {
            float Gx = get_pixel(gx, x, y, 0);
            float Gy = get_pixel(gy, x, y, 0);

            float G_mag = sqrt(Gx * Gx + Gy * Gy);
            float G_theta = atan2(Gy, Gx);

            set_pixel(color_sobel, x, y, 0, G_mag);
            set_pixel(color_sobel, x, y, 1, 0);
            set_pixel(color_sobel, x, y, 0, G_theta);
        }
    }

    image fgauss = make_gaussian_filter(2);
    color_sobel = convolve_image(color_sobel, fgauss, 1);

    return color_sobel;
}