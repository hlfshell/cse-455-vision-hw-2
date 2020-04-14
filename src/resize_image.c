#include <math.h>
#include "image.h"
#include <stdio.h>

float nn_interpolate(image im, float x, float y, int c)
{
    float value = get_pixel(im, round(x), round(y), c);
    
    return value;
}

image nn_resize(image im, int w, int h)
{
    /*
        a*x + b = x_orig
        
        and
        a* -.5 + b = -.5
        a*(w-.5) + b = (w_orig-.5)

        SO

        b = .5a -.5
        a*(w-.5) + .5a -.5 = w_orig -.5
        aw-.5a +.5a = w_org
        aw = w_org
        a = w_org / w

        thus

        ...repeat for height.
        
    */
   
    float ax, bx, ay, by, ix, iy;

    ax = (float) im.w / (float) w;
    bx = 0.5 * ((float) im.w / (float) w) - .5;

    ay = (float) im.h / (float) h;
    by = 0.5 * ((float) im.h / (float) h) - .5;

    image newImage = make_image(w, h, im.c);

    for(int c = 0; c < im.c; c ++){    
        for(int y = 0; y < h; y ++){
            for(int x = 0; x < w; x ++){
                ix = ax * x + bx;
                iy = ay * y + by;
                set_pixel(newImage, x, y, c, nn_interpolate(im, ix, iy, c));
            }
        }
    }

    return newImage;
}

typedef struct {
    float x;
    float y;
} point;

float bilinear_interpolate(image im, float x, float y, int c)
{

    point v1 = {floor(x), floor(y)};
    point v2 = {ceil(x) , floor(y)};
    point v3 = {floor(x), ceil(y)};
    point v4 = {ceil(x), ceil(y)};
    
    float v1Value, v2Value, v3Value, v4Value;

    v1Value = get_pixel(im, v1.x, v1.y, c);
    v2Value = get_pixel(im, v2.x, v2.y, c);
    v3Value = get_pixel(im, v3.x, v3.y, c);
    v4Value = get_pixel(im, v4.x, v4.y, c);

    float d1, d2, d3, d4;

    d1 = x - v1.x;
    d2 = v2.x - x;
    d3 = y - v1.y;
    d4 = v4.y - y;

    float a1, a2, a3, a4;

    a1 = d2 * d4;
    a2 = d1 * d4;
    a3 = d2 * d3;
    a4 = d1 * d3;

    float result = (v1Value * a1) + (v2Value * a2) + (v3Value * a3) + (v4Value * a4);

    return result;
}

image bilinear_resize(image im, int w, int h)
{
       /*
        a*x + b = x_orig
        
        and
        a* -.5 + b = -.5
        a*(w-.5) + b = (w_orig-.5)

        SO

        b = .5a -.5
        a*(w-.5) + .5a -.5 = w_orig -.5
        aw-.5a +.5a = w_org
        aw = w_org
        a = w_org / w

        thus

        ...repeat for height.
        
    */
   
    float ax, bx, ay, by, ix, iy;

    ax = (float) im.w / (float) w;
    bx = 0.5 * ((float) im.w / (float) w) - .5;

    ay = (float) im.h / (float) h;
    by = 0.5 * ((float) im.h / (float) h) - .5;

    image newImage = make_image(w, h, im.c);

    for(int c = 0; c < im.c; c ++){    
        for(int y = 0; y < h; y ++){
            for(int x = 0; x < w; x ++){
                ix = ax * x + bx;
                iy = ay * y + by;
                set_pixel(newImage, x, y, c, bilinear_interpolate(im, ix, iy, c));
            }
        }
    }

    return newImage;

}

