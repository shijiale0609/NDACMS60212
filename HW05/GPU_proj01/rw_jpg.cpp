//JPEG image processing 
// https://github.com/JTODR/JPEG-Threading-Decompression-To-PPM/blob/master/JPEG%20compression%20threading.c#L91

//argv[0] is name of this program 
//argv[1] is input .jpg image
//eg:  ./run smallimage.jpg 

// Sunflowers.jpg: 2,434 × 1,697 pixels, file size: 2.67 MB, MIME type: image/jpeg
// https://commons.wikimedia.org/wiki/File:Sunflower_from_Silesia2.jpg

#include "rw_jpg.h"


using namespace std; 

/**
**/

/*Compilation:  icpc read_jpg.cpp -ljpeg */
/**
int main (int argc, char *argv[]) {

	int rc, i, j;
	
	
        // Variables for output buffer
	unsigned long bmp_size;		//size of buffer
	unsigned char *image_data;	//output buffer - will be used to write to the output file
	int row_stride, width, height, pixel_size;
        int red, green, blue; 
        int pixel = 0; 

        const char *filename = argv[1];	
        bundle  RGB_bundle;

        read_jpg(filename, &RGB_bundle); 
        pixel_size = RGB_bundle.num_channels; 
        
        for(i = 0; i < RGB_bundle.height; i++)
        {
            for(j = 0; j < RGB_bundle.width; j++)
            {
                red = RGB_bundle.image_data[pixel*pixel_size];
		blue = RGB_bundle.image_data[pixel*pixel_size +1];
		green = RGB_bundle.image_data[pixel*pixel_size +2];
                pixel++; 
                cout<<red<<","<<blue<<","<<green<<endl;
            }
        }
}
**/


int read_jpg(const char *filename, bundle *o_data) {

	int rc, j;
	
	// Variables for the source jpg
	struct stat file_info;		

	unsigned long jpg_size;		//size of image
	unsigned char *jpg_buffer;	//input buffer

        // Variables for the decompressor - from jpeglib.h
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

        // Variables for output buffer
	unsigned long bmp_size;		//size of buffer
	unsigned char *image_data;	//output buffer - will be used to write to the output file
	int row_stride, width, height, pixel_size;

        // Load the jpeg data from the .jpg file
        // const char * filename = argv[1];	
	rc = stat(filename, &file_info);
	
	jpg_size = file_info.st_size;	//st_size is part of struct stat that returns the filesize
	jpg_buffer = (unsigned char*) malloc(jpg_size);	


	int fd = open(filename, O_RDONLY);	//O_RDONLY is makes the file read only
	
	int i = 0;
	while (i < jpg_size) {
		rc = read(fd, jpg_buffer + i, jpg_size - i);	
		i += rc;
	}
	close(fd);

	cinfo.err = jpeg_std_error(&jerr);	
	jpeg_create_decompress(&cinfo);

	jpeg_mem_src(&cinfo, jpg_buffer, jpg_size);

        //Read the jpeg header, it will always be true
	rc = jpeg_read_header(&cinfo, TRUE);

        //Compress the image into cinfo
	jpeg_start_decompress(&cinfo);
	
	o_data->width = width = cinfo.output_width;
	o_data->height = height = cinfo.output_height;
	o_data->num_channels = pixel_size = cinfo.output_components;

	printf("Image width: %d pixels\n", width);
	printf("Image height: %d pixels\n", height);
	printf("Image channels per pixel: %d \n", pixel_size);
	
        // Set up the output buffer
        bmp_size = width * height * pixel_size;
	image_data = (unsigned char*) malloc(bmp_size);

	//physical row width of array buffer: 3 channels * width of image, bytes per pixel
	row_stride = width * pixel_size;	
	while (cinfo.output_scanline < cinfo.output_height) {	
		unsigned char *buffer_array[1];		//jpeg_read_scanlines takes an array of buffers
		buffer_array[0] = image_data + (cinfo.output_scanline) * row_stride;

		// this will take 1 line at a time, the 1 ensures high quality
		// not having multiple buffers going at the same time
		jpeg_read_scanlines(&cinfo, buffer_array, 1);
	}
	
	jpeg_finish_decompress(&cinfo);

	jpeg_destroy_decompress(&cinfo);
	free(jpg_buffer);
     
        o_data->image_data = image_data; 

        return true;
}


/*
 * Modified based on:
 * https://github.com/LuaDist/libjpeg/blob/master/example.c
 */
/*
 * filename: name of the output file. 
 * o_data: jpeg image data.
 * */
int write_jpg(const char *filename, bundle *o_data) 
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE * outfile;		/* target file */
    JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
    int row_stride;		/* physical row width in image buffer */

  /* Step 1: allocate and initialize JPEG compression object */
    cinfo.err = jpeg_std_error(&jerr);
  /* Now we can initialize the JPEG compression object. */
    jpeg_create_compress(&cinfo);

  /* Step 2: specify data destination (eg, a file) */
    if ((outfile = fopen(filename, "wb")) == NULL) {
        fprintf(stderr, "can't open %s\n", filename);
        exit(1);
    }
    jpeg_stdio_dest(&cinfo, outfile);

  /* Step 3: set parameters for compression */

    cinfo.image_width = o_data->width; 	/* image width and height, in pixels */
    cinfo.image_height = o_data->height;
    cinfo.input_components = o_data->num_channels;		/* # of color components per pixel, default 3 */
    cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */

    jpeg_set_defaults(&cinfo);

    /* Now you can set any non-default parameters you wish to.
     * Here we just illustrate the use of quality (quantization table) scaling:
     */    
    jpeg_set_quality(&cinfo, 100, TRUE /* limit to baseline-JPEG values */);

    /* Step 4: Start compressor */
    /* TRUE ensures that we will write a complete interchange-JPEG file.
     * Pass TRUE unless you are very sure of what you're doing.
     */
    jpeg_start_compress(&cinfo, TRUE);

    /* Step 5: while (scan lines remain to be written) */
    row_stride = o_data->width * o_data->num_channels;	/* JSAMPLEs per row in image_buffer */
    while (cinfo.next_scanline < cinfo.image_height) {
        /* jpeg_write_scanlines expects an array of pointers to scanlines.
         * Here the array is only one element long, but you could pass
         * more than one scanline at a time if that's more convenient.
         */
        row_pointer[0] = & o_data->image_data[cinfo.next_scanline * row_stride];
        (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

      /* Step 6: Finish compression */

    jpeg_finish_compress(&cinfo);
    /* After finish_compress, we can close the output file. */
    fclose(outfile);

    /* Step 7: release JPEG compression object */

    /* This is an important step since it will release a good deal of memory. */
    jpeg_destroy_compress(&cinfo);

    return true;

    /* done! */
}


