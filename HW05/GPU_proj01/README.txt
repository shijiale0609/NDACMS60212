Compiling options: 
1) nvcc matr_multiply.cu rw_jpg.cpp  main.cpp Vector.cpp -ljpeg -std=c++11 -o prog
2) use the Makefile

main() function in main.cpp

To run the program: ./a.out Sunflower.jpg

What's done on CPU:
1. The current code read in this image by calling function 

read_jpg(filename, &RGB_bundle);

On return, the RGB_bundle struct stores the RGB values of the image by array pointed by member pointer image_data of the struct.  
The double for loop shows how to extract RGB values for each pixels. 

2. Use matrices (Red, Blue, Green) to store RGB values respectively. Each matric only saves one color. 
3. Use the following 3 by 3 filter to blur the image: 
     1/9{{1 1 1}; {1 1 1}; {1 1 1}}. 
    Multiply each element of the kernel with its corresponding element of the image matrix (the one which is overlapped with it)
Sum up all product outputs and put the result in the output matrix.
4. Write the blurred image to a file by call function write_jpg(const char*,bundle*);

5. Function cuda_kernel_wrapper() is declared in cuda_wrapper.h, and defined in matr_multiply.cu.
   It is a C function in which a kernel is lauched.  It is called by main().

6. The Vector class is not necessary for this project.

-------------------------
Goal. Implement a CUDA kernel function for filtering the jpeg image: Sunflower.jpg
-------------------------

Note: matr_multiply.cu was originally designed for doing matrix matrix multiplication on GPU. 

What's needed:
1. Modify arguments of cuda_kernel_wrapper() to pass matrices (Red, Blue, Green).
2. If you wish to use struct cu_Matrix to store rgb values of the picture, change 

typedef struct {
    int width;
    int height;
    float* elements;
} cu_Matrix;

to 

typedef struct {
    int            width;
    int            height;
    unsigned char* elements;
} cu_Matrix;

3. Use 2D block decomposition on the picture to yield parallel taskes.

4. According to 2D block decomposition of taskes, use 2D grid of blocks and 2D block of threads structure to lauch kernel function.

5. The kernel function does the filtering.  Each thread filters one pixel of the image. 

6. Banchmark the GPU time used by the kernel.
-------------------------



 

