#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    float sum = 0;

    for(int c = 0; c < im.c; c++){
        for(int y = 0; y < im.h; y++){
            for(int x = 0; x < im.w; x++){
                float value = get_pixel(im, x, y, c);
                sum += value;
            }
        }
    }

    for(int c = 0; c < im.c; c++){
        for(int y = 0; y < im.h; y++){
            for(int x = 0; x < im.w; x++){
                float value = get_pixel(im, x, y, c);
                set_pixel(im, x, y, c, value / sum);
            }
        }
    }
}

image make_box_filter(int w)
{
    image filter = make_image(w, w, 1);
    float total = w * w;
    for(int i = 0; i < w * w; i++){
        filter.data[i] = 1 / total;
    }
    return filter;
}

image convolve_image(image im, image filter, int preserve)
{
    assert(im.c == filter.c || filter.c == 1);

    image convolved;
    int fOffset = filter.w / 2;
    float sum = 0;
    float fValue;
    int nx, ny;
    float imValue;

    if(preserve == 1){
        convolved = make_image(im.w, im.h, im.c);

        for(int c = 0; c < im.c; c++){
            for(int y = 0; y < im.h; y++){
                for(int x = 0; x < im.w; x++){

                    sum = 0;

                    for(int fy = 0; fy < filter.h; fy++){
                        for(int fx = 0; fx < filter.w; fx++){
                            fValue = get_pixel(filter, fx, fy, 0);
                            
                            nx = x + (-1 * fOffset ) + fx;
                            ny = y + (-1 * fOffset ) + fy;

                            // //Zero padding = results in a 0 to
                            // //this piece of the convolution
                            // if(x < 0 || y < 0){
                            //     continue;
                            // } else if(x >= im.w || y >= im.h) {
                            //     continue;
                            // }

                            imValue = get_pixel(im, nx, ny, c);
                            sum += fValue * imValue;
                        }
                    }
                    set_pixel(convolved, x, y, c, sum);
                }
            }
        }

    } else {
        convolved = make_image(im.w, im.h, 1);

        for(int y = 0; y < im.h; y++){
            for(int x = 0; x < im.w; x++){
                sum = 0;
                
                for(int fy = 0; fy < filter.h; fy++){
                    for(int fx = 0; fx < filter.w; fx++){
                        fValue = get_pixel(filter, fx, fy, 0);

                        nx = x + (-1 * fOffset) + fx;
                        ny = y + (-1 * fOffset) + fy;

                        for(int c = 0; c < im.c; c++){
                            imValue = get_pixel(im, nx, ny, c);
                            // printf("(%d, %d, %d) C (%d, %d, %d)=> (%d,%d) :: %f * %f = %f\n", x, y, c, fx, fy, 0, nx, ny, fValue, imValue, sum);
                            sum += fValue * imValue;
                        }
                    }
                }
                set_pixel(convolved, x, y, 0, sum);
            }
        }
    }

    return convolved;
}

image make_highpass_filter()
{
    image filter = make_image(3, 3, 1);
    
    filter.data[0] = 0;
    filter.data[1] = -1;
    filter.data[2] = 0;

    filter.data[3] = -1;
    filter.data[4] = 4;
    filter.data[5] = -1;

    filter.data[6] = 0;
    filter.data[7] = -1;
    filter.data[8] = 0;

    return filter;
}

image make_sharpen_filter()
{
    image filter = make_image(3, 3, 1);

    filter.data[0] = 0;
    filter.data[1] = -1;
    filter.data[2] = 0;

    filter.data[3] = -1;
    filter.data[4] = 5;
    filter.data[5] = -1;

    filter.data[6] = 0;
    filter.data[7] = -1;
    filter.data[8] = 0;

    return filter;
}

image make_emboss_filter()
{
    image filter = make_image(3, 3, 1);

    filter.data[0] = -2;
    filter.data[1] = -1;
    filter.data[2] = 0;

    filter.data[3] = -1;
    filter.data[4] = 1;
    filter.data[5] = 1;

    filter.data[6] = 0;
    filter.data[7] = 1;
    filter.data[8] = 2;

    return filter;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: TODO

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: TODO

image make_gaussian_filter(float sigma)
{
    int size = sigma * 6;
    if(size % 2 == 0) size += 1;

    /*
        a*e^b
        a = 1/(2*pi*sigma^2)
        b = -(x^2+y^2)/(2*sigma^2)
    */ 

   image filter = make_image(size, size, 1);

   float a, b, value, ox, oy;

   for(int y= 0; y < filter.h; y++){
       for(int x = 0; x < filter.w; x++){
           ox = x - (size/2);
           oy = y - (size/2);

           a = 1 / (TWOPI * pow(sigma, 2));
           b = -1 * ( (pow(ox,2) + pow(oy, 2)) / (2 * pow(sigma, 2)) );
           value = a * exp(b);

           set_pixel(filter, x, y, 0, value);
       }
   }

    l1_normalize(filter);

    return filter;
}

image add_image(image a, image b)
{
    assert(a.w == b.w);
    assert(a.h == b.h);
    assert(a.c == b.c);

    image result = make_image(a.w, a.h, a.c);
    
    float aValue, bValue;

    for(int c = 0; c < result.c; c++){
        for(int y = 0; y < result.h; y++){
            for(int x = 0; x < result.w; x++){
                aValue = get_pixel(a, x, y, c);
                bValue = get_pixel(b, x, y, c);
                set_pixel(result, x, y, c, aValue + bValue);
            }
        }
    }

    return result;
}

image sub_image(image a, image b)
{
    assert(a.w == b.w);
    assert(a.h == b.h);
    assert(a.c == b.c);

    image result = make_image(a.w, a.h, a.c);
    
    float aValue, bValue;

    for(int c = 0; c < result.c; c++){
        for(int y = 0; y < result.h; y++){
            for(int x = 0; x < result.w; x++){
                aValue = get_pixel(a, x, y, c);
                bValue = get_pixel(b, x, y, c);
                set_pixel(result, x, y, c, aValue - bValue);
            }
        }
    }

    return result;
}

image make_gx_filter()
{
    image filter = make_image(3, 3, 1);

    filter.data[0] = -1;
    filter.data[1] = 0;
    filter.data[2] = 1;

    filter.data[3] = -2;
    filter.data[4] = 0;
    filter.data[5] = 2;

    filter.data[6] = -1;
    filter.data[7] = 0;
    filter.data[8] = 1;

    return filter;
}

image make_gy_filter()
{
    image filter = make_image(3, 3, 1);

    filter.data[0] = -1;
    filter.data[1] = -2;
    filter.data[2] = -1;

    filter.data[3] = 0;
    filter.data[4] = 0;
    filter.data[5] = 0;

    filter.data[6] = 1;
    filter.data[7] = 2;
    filter.data[8] = 1;

    return filter;
}

void feature_normalize(image im)
{
    float min, max;

    min = im.data[0];
    max = im.data[0];

    for(int i = 0; i < im.w * im.h * im.c; i++){
        if(im.data[i] < min){
            min = im.data[i];
        } else if(im.data[i] > max){
            max = im.data[i];
        }
    }
    
    for(int i = 0; i < im.w * im.h * im.c; i++){
        im.data[i] = (im.data[i] - min) / (max - min);
    }
}

image *sobel_image(image im)
{
    image sobel_x, sobel_y;
    sobel_x = make_gx_filter();
    sobel_y = make_gy_filter();

    image gx, gy;
    gx = convolve_image(im, sobel_x, 0);
    gy = convolve_image(im, sobel_y, 0);


    image magnitude, direction;
    magnitude = make_image(im.w, im.h, 1);
    direction = make_image(im.w, im.h, 1);

    float gxV, gyV, magnitudeValue, directionValue;

    for(int y = 0; y < im.h; y++){
        for(int x = 0; x < im.w; x++){
            gxV = get_pixel(gx, x, y, 0);
            gyV = get_pixel(gy, x, y, 0);

            magnitudeValue = sqrt((gxV * gxV) + (gyV * gyV));
            directionValue = atan2(gyV, gxV);

            set_pixel(magnitude, x, y, 0, magnitudeValue);
            set_pixel(direction, x, y, 0, directionValue);
        }
    }

    image *result = calloc(2, sizeof(image));

    result[0] = magnitude;
    result[1] = direction;

    return result;
}

image colorize_sobel(image im)
{
    image result = copy_image(im);

    image *sobel_return = sobel_image(result);
    image magnitude = sobel_return[0];
    image direction = sobel_return[1];

    feature_normalize(magnitude);
    feature_normalize(direction);

    rgb_to_hsv(result);

    //For channels 0, 1 (hue, saturation), we apply the
    //direction,gradient respectively.
    for(int y = 0; y < im.h; y++){
        for(int x = 0; x < im.w; x++){
            set_pixel(result, x, y, 1, get_pixel(magnitude, x, y, 0));
            set_pixel(result, x, y, 2, get_pixel(magnitude, x, y, 0));
            set_pixel(result, x, y, 0, get_pixel(direction, x, y, 0));
        }
    }

    hsv_to_rgb(result);

    return result;
}
