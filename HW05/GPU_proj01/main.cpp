#include <iostream>
#include <new>
#include <cstddef>
#include <fstream>
#include "rw_jpg.h"
#include "pointer_2d_matrix.h"
#include "cuda_wrapper.h"

//argv[0] is name of this program
//argv[1] is input .jpg image

//eg:  ./a.out smallimage.jpg

void test_wrapper( Matrix); 
using namespace std;

int main(int argc, char *argv[])
{
        double **A;
        int    i,j,k,l;

    /*Begin: read in a jpg image*/
        int pixel_size;
        int red, green, blue;
        int pixel = 0;
        int rows, cols;
        int debug = false; 

        const char *filename = argv[1];
        bundle  RGB_bundle;

        read_jpg(filename, &RGB_bundle);

        pixel_size = RGB_bundle.num_channels;
        int Width=RGB_bundle.width;
        int Height=RGB_bundle.height;
        
       std::cout << "Width=" << Width << "," << "Height" << Height << std::endl;
//create 3 matrices that store the RGB values of each pixel
        Matrix Red(Height,Width),Blue(Height,Width),Green(Height,Width);

        float  *data_Red, *data_Blue, *data_Green;
        data_Red = (float *)malloc(Width*Height*sizeof(float));
        data_Blue = (float *)malloc(Width*Height*sizeof(float));
        data_Green = (float *)malloc(Width*Height*sizeof(float));

        // cout<<"image Width = "<<Width<<", Height = "<< Width<<endl;

        for(i = 0; i <Height ; i++)
        {
            for(j = 0; j < Width; j++)
            {
                Red(i,j) = RGB_bundle.image_data[pixel*pixel_size];
                Blue(i,j) = RGB_bundle.image_data[pixel*pixel_size +1];
                Green(i,j) = RGB_bundle.image_data[pixel*pixel_size +2];
                
                pixel++;
                //cout<<red<<","<<blue<<","<<green<<endl;
            }
        }

        pixel=0;
        for(i = 0; i <Height ; i++)
        {
            for(j = 0; j < Width; j++)
            {
                
                data_Red[i*Width + j] = RGB_bundle.image_data[pixel*pixel_size];
                data_Blue[i*Width + j] = RGB_bundle.image_data[pixel*pixel_size +1];
                data_Green[i*Width + j] = RGB_bundle.image_data[pixel*pixel_size +2]; 
                pixel++;
            }
        }
        //test_wrapper(Red);

//Create the matrix filter
	Matrix matFilter(FILTER_SIZE,FILTER_SIZE);
	for(i=0;i<FILTER_SIZE;i++)
	{
            for(j=0;j<FILTER_SIZE;j++)
	    {
	        matFilter(i,j)=(double)(1/9);
	    }
	}

//Use the filter to blur the image


// Here, I did not do a good job to take care of pixels on
// the first and last rows and first and last columns. 
//first create loops to move filter along the grid
	for(i=1;i<Height-1;i++){
	    for(j=1;j<Width-1;j++){
		pixel=j+i*Width;

		red=0;
		blue=0;
		green=0;
	    
//iterate through each pixel enclosed by the filter
		for(k=0;k<FILTER_SIZE;k++){
		    for(l=0;l<FILTER_SIZE;l++){

			red=red+(int)(matFilter(k,l)*Red(i+k-1,j+l-1));
			blue=blue+(int)(matFilter(k,l)*Blue(i+k-1,j+l-1));
			green=green+(int)(matFilter(k,l)*Green(i+k-1,j+l-1));
		    }
		}
//assign 
	   	RGB_bundle.image_data[pixel*pixel_size]=red;
            	RGB_bundle.image_data[pixel*pixel_size +1]=blue;
            	RGB_bundle.image_data[pixel*pixel_size +2]=green; 

		 }
	}

    /*End: read in a jpg image*/

    /*save jpeg file*/
    write_jpg("Sunflower_2.jpg", &RGB_bundle);
    /*END: save jpeg file*/



    // this eventually calls cuda kernel
    cout << "Process Red:" << endl;
    cuda_kernel_wrapper(data_Red, Width, Height);
    cout << "Process Blue:" << endl;
    cuda_kernel_wrapper(data_Blue, Width, Height); 
    cout << "Process Green:" << endl;
    cuda_kernel_wrapper(data_Green, Width, Height); 

     pixel=0;
     for(i = 0; i < Height; i++)
        {
            for(j = 0; j < Width; j++)
            {
	   	RGB_bundle.image_data[pixel*pixel_size] = data_Red[i*Width+j];
            	RGB_bundle.image_data[pixel*pixel_size +1] = data_Blue[i*Width+j];
            	RGB_bundle.image_data[pixel*pixel_size +2] = data_Green[i*Width+j]; 
                pixel++;
            }
    }
    /*save jpeg file*/
    write_jpg("Sunflower_3.jpg", &RGB_bundle);
    /*END: save jpeg file*/
    
    free(RGB_bundle.image_data); 

}

void test_wrapper( Matrix mat){
} 

