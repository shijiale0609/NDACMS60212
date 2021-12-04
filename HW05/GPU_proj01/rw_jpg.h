#if !defined(_READ_JPG_H)
#define _READ_JPG_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     //needed for read/ write functions
#include <sys/stat.h>   //used to determine details of a file to be read in
#include <sys/time.h>

#include <jpeglib.h>
#include<iostream>

using namespace std;

struct bundle{
        unsigned char *image_data; /*pixel RGB values stored in rgb value of pixel by 
                                     rgb value of pixel format. 
                                    size of the array = width*height*num_channels*/
        int width, height; /* number of pixels in each direction*/
        int num_channels;  /*# of color components per pixel, default 3 */
};
typedef struct bundle bundle;

int read_jpg(const char*, bundle*);
int write_jpg(const char*, bundle*);


#endif 
