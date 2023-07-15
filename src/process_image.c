#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    if (x < 0 || x >= im.w || y < 0 || y >= im.h || c < 0 || c >= im.c)
    {
        // Implementing the "clamp" padding strategy
        int xa = (x < 0) ? 0 : ((x >= im.w) ? im.w - 1 : x);
        int ya = (y < 0) ? 0 : ((y >= im.h) ? im.h - 1 : y);
        int ca = (c < 0) ? 0 : ((c >= im.c) ? im.c - 1 : c);
        return im.data[xa + im.w * ya + im.w * im.h * ca];
    }
    else
        return im.data[x + im.w * y + im.w * im.h * c];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    if (x < 0 || x >= im.w || y < 0 || y >= im.h || c < 0 || c >= im.c)
        return;
    else
        im.data[x + im.w * y + im.w * im.h * c] = v;
    return;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    memcpy(copy.data, im.data, im.w * im.h * im.c * sizeof(float));
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    for (int i = 0; i < im.w * im.h; i++)
    {
        gray.data[i] = 0.299 * im.data[i] + 0.587 * im.data[i + im.w * im.h] + 0.114 * im.data[i + 2 * im.w * im.h];
        // Using the given formula Y' = 0.299 R' + 0.587 G' + .114 B' to convert RGB to grayscale
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    for (int i = 0; i < im.w * im.h; i++)
    {
        im.data[i + im.w * im.h * c] += v; // Adding is fine, since we are shifting
    }
}

// Adding a scaling function

void scale_image(image im, int c, float v)
{
    for (int i = 0; i < im.w * im.h; i++)
    {
        im.data[i + im.w * im.h * c] *= v; // Multiplying since we are scaling
    }
}

void clamp_image(image im)
{
    for (int i = 0; i < im.w * im.h * im.c; i++)
    {
        if (im.data[i] > 1)
            im.data[i] = 1;
        else if (im.data[i] < 0)
            im.data[i] = 0;
    }
}

// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
}

void rgb_to_hsv(image im)
{
    for (int i = 0; i < im.w * im.h; i++)
    {
        float V = three_way_max(im.data[i], im.data[i + im.w * im.h], im.data[i + 2 * im.w * im.h]);
        float m = three_way_min(im.data[i], im.data[i + im.w * im.h], im.data[i + 2 * im.w * im.h]);
        float S = 0, C = V - m;
        if (fabs(V - 0) < .00001)
            S = 0;
        else
            S = (V - m) / V;
        float R = im.data[i], G = im.data[i + im.w * im.h], B = im.data[i + 2 * im.w * im.h];
        float H_ = 0;
        if (fabs(C - 0) < .00001)
            H_ = 0;
        else if (fabs(V - R) < .00001)
            H_ = (G - B) / C;
        else if (fabs(V - G) < .00001)
            H_ = (B - R) / C + 2;
        else if (fabs(V - B) < 0.00001)
            H_ = (R - G) / C + 4;

        float H = 0;
        if (H_ < 0)
            H = H_ / 6 + 1;
        else
            H = H_ / 6;

        im.data[i] = H;
        im.data[i + im.w * im.h] = S;
        im.data[i + 2 * im.w * im.h] = V;
    }
    return;
}

void hsv_to_rgb(image im)
{
    // From, Color Gamut Transform Pairs (Smith A. R., 1978)

    for (int i = 0; i < im.w * im.h; i++)
    {
        float H = im.data[i];
        float S = im.data[i + im.w * im.h];
        float V = im.data[i + 2 * im.w * im.h];

        float R, G, B;
        float H_ = 6 * H;
        int I = floor(H_);
        float F = H_ - I;
        float M = V * (1 - S);
        float N = V * (1 - S * F);
        float K = V * (1 - S * (1 - F));

        switch (I)
        {
        case 0:
            R = V;
            G = K;
            B = M;
            break;
        case 1:
            R = N;
            G = V;
            B = M;
            break;
        case 2:
            R = M;
            G = V;
            B = K;
            break;
        case 3:
            R = M;
            G = N;
            B = V;
            break;
        case 4:
            R = K;
            G = M;
            B = V;
            break;
        case 5:
            R = V;
            G = M;
            B = N;
            break;
        default:
            R = 0;
            G = 0;
            B = 0;
            break;
        }

        im.data[i] = R;
        im.data[i + im.w * im.h] = G;
        im.data[i + 2 * im.w * im.h] = B;
    }
    return;
}

void rgb_to_xyz(image im)
{
    for (int i = 0; i < im.w * im.h; i++)
    {
        float R = im.data[i];
        float G = im.data[i + im.w * im.h];
        float B = im.data[i + 2 * im.w * im.h];

        // From https://www.cs.rit.edu/~ncs/color/t_convert.html

        float X = 0.412453 * R + 0.357580 * G + 0.180423 * B;
        float Y = 0.212671 * R + 0.715160 * G + 0.072169 * B;
        float Z = 0.019334 * R + 0.119193 * G + 0.950227 * B;

        im.data[i] = X;
        im.data[i + im.w * im.h] = Y;
        im.data[i + 2 * im.w * im.h] = Z;
    }
    return;
}

void xyz_to_luv(image im)
{
    for (int i = 0; i < im.w * im.h; i++)
    {
        float X = im.data[i];
        float Y = im.data[i + im.w * im.h];
        float Z = im.data[i + 2 * im.w * im.h];

        // White point as reference (d65 taken here)
        float Xn = 0.95;
        float Yn = 1.0;
        float Zn = 1.09;

        float L = 0, u = 0, v = 0;
        float un = 4 * Xn / (Xn + 15 * Yn + 3 * Zn);
        float vn = 9 * Yn / (Xn + 15 * Yn + 3 * Zn);

        // (6/29)^3 = 0.008856

        if (Y / Yn <= 0.008856)
            L = 903.3 * Y / Yn;
        else
            L = 116 * pow(Y / Yn, 1.0 / 3.0) - 16;

        float u_ = (4 * X) / (X + 15 * Y + 3 * Z);
        float v_ = (9 * Y) / (X + 15 * Y + 3 * Z);
        u = 13 * L * (u_ - un);
        v = 13 * L * (v_ - vn);

        im.data[i] = L;
        im.data[i + im.w * im.h] = u;
        im.data[i + 2 * im.w * im.h] = v;
    }
    return;
}

// what for luv to hcl(cylindrical coordinates) ???

void luv_to_xyz(image im)
{
    for (int i = 0; i < im.w * im.h; i++)
    {
        float L = im.data[i];
        float u = im.data[i + im.w * im.h];
        float v = im.data[i + 2 * im.w * im.h];

        float Xn = 0.95;
        float Yn = 1.0;
        float Zn = 1.09;

        float un = 4 * Xn / (Xn + 15 * Yn + 3 * Zn);
        float vn = 9 * Yn / (Xn + 15 * Yn + 3 * Zn);

        float u_ = u / (13 * L) + un;
        float v_ = v / (13 * L) + vn;

        float Y = 0;
        if (L <= 8)
            Y = Yn * L * pow(3.0 / 29.0, 3);
        else
            Y = Yn * pow((L + 16) / 116, 3);

        float X = 0, Z = 0;
        if (fabs(v_) < .00001)
        {
            X = 0;
            Z = 0;
        }
        else
        {
            X = Y * 2.25 * u_ / v_;
            Z = Y * (3 - 0.75 * u_ - 5 * v_) / v_;
        }

        im.data[i] = X;
        im.data[i + im.w * im.h] = Y;
        im.data[i + 2 * im.w * im.h] = Z;
    }
    return;
}

void xyz_to_rgb(image im)
{
    for (int i = 0; i < im.w * im.h; i++)
    {
        float X = im.data[i];
        float Y = im.data[i + im.w * im.h];
        float Z = im.data[i + 2 * im.w * im.h];

        float R = 3.240479 * X - 1.537150 * Y - 0.498535 * Z;
        float G = -0.969256 * X + 1.875992 * Y + 0.041556 * Z;
        float B = 0.055648 * X - 0.204043 * Y + 1.057311 * Z;

        im.data[i] = R;
        im.data[i + im.w * im.h] = G;
        im.data[i + 2 * im.w * im.h] = B;
    }
    return;
}