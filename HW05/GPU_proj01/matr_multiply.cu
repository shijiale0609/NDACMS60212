#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <curand_kernel.h>
#include <iostream>
#include <assert.h>

//#include <new>
//#include <cstddef>
//#include <fstream>
//#include "pointer_2d_matrix.h"
#include "cuda_wrapper.h"

// Thread block size
//#define BLOCK_SIZE  16  // number of threads in a direction of the block
//#define M_WIDTH     512 // number of columns
//#define M_HEIGHT    512 // number of rows

#define TILE_W      14
#define TILE_H      14
#define R           1                   // filter radius
#define D           (R*2+1)             // filter diameter
#define S           (D*D)               // filter size
#define BLOCK_W     (TILE_W+(2*R))
#define BLOCK_H     (TILE_H+(2*R))


// I take the following links as my reference for cuda_kernel_wrapper
// https://stackoverflow.com/questions/39336574/box-filter-with-cuda-c
// https://www.nvidia.com/content/nvision2008/tech_presentations/Game_Developer_Track/NVISION08-Image_Processing_and_Video_with_CUDA.pdf
__global__ void d_filter(float *g_idata, float *g_odata, unsigned int width, unsigned int height)
{

  __shared__ int smem[BLOCK_W*BLOCK_H*BLOCK_H];

    int x = blockIdx.x*TILE_W + threadIdx.x - R;
    int y = blockIdx.y*TILE_H + threadIdx.y - R;
    // clamp to edge of image
    x = max(0, x);
    x = min(x, width-1);
    y = max(y, 0);
    y = min(y, height-1);
    unsigned int index = y*width + x;
    unsigned int bindex = threadIdx.y*blockDim.y+threadIdx.x;
    // each thread copies its pixel of the block to shared memory
    smem[bindex] = g_idata[index];
    __syncthreads();
    // only threads inside the apron will write results
    if ((threadIdx.x >= R) && (threadIdx.x < (BLOCK_W-R)) && (threadIdx.y >= R) && (threadIdx.y < (BLOCK_H-R))) {
        float sum = 0;
        for(int dy=-R; dy<=R; dy++) {
            for(int dx=-R; dx<=R; dx++) {
                float i = smem[bindex + (dy*blockDim.x) + dx];
                sum += i;
            }
        }
        g_odata[index] = sum / S;
    }

}

int cuda_kernel_wrapper(float *data, int image_Width, int image_Height){

    float   *d_idata, *d_odata;
    unsigned int w = image_Width, h = image_Height , channels= 1;

    //printf("initial results:\n");
    //for (int i = 1000; i < 1016; i++){
    //  for (int j = 1000; j < 1016; j++) printf("%f ", data[i*w+j]);
    //  printf("\n");}

    unsigned int numElements = w*h*channels;
    size_t datasize = numElements * sizeof(float);
    cudaError_t err = cudaSuccess;    

    // Allocate the Device Memory
    printf("Allocate Devicememory for data\n");

    err = cudaMalloc((void **)&d_idata, datasize);
    if ( err != cudaSuccess)
    {
        fprintf(stderr, "Failed to allocate device memory for idata (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    err = cudaMalloc((void **)&d_odata, datasize);
    if ( err != cudaSuccess)
    {
        fprintf(stderr, "Failed to allocate device memory for odata (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    // Copy to device
    printf("Copy idata from the host memory to the CUDA device\n");
    err =cudaMemcpy(d_idata, data, datasize, cudaMemcpyHostToDevice);
    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to copy idata from host to device (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    // Launch Kernel
    dim3 threadsPerBlock(BLOCK_W, BLOCK_H);
    int GRID_W = w/TILE_W+1;
    int GRID_H = h/TILE_H+1;
    dim3 blocksPerGrid(GRID_W, GRID_H);
    //dim3 blocksPerGrid((w+threadsPerBlock.x-1)/threadsPerBlock.x, (h+threadsPerBlock.y-1)/threadsPerBlock.y);
    printf("CUDA kernel launch with %d,%d blocks of %d,%d threads\n", blocksPerGrid.x, blocksPerGrid.y, threadsPerBlock.x, threadsPerBlock.y);

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);
    
    // kernel call
    d_filter<<<blocksPerGrid, threadsPerBlock>>>(d_idata, d_odata, w,h);

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    float elapseTime;
    cudaEventElapsedTime(&elapseTime, start, stop); 
    
    printf("Time to run the kernel: %f ms\n", elapseTime);

    // Copy data from device to host
    printf("Copy odata from the CUDA device to the host memory\n");
    cudaMemcpy(data, d_odata, datasize, cudaMemcpyDeviceToHost);

    // Free Device memory
    printf("Free Device memory\n");
    cudaFree(d_idata);
    cudaFree(d_odata);

    //printf("final results:\n");
    //for (int i = 1000; i < 1016; i++){
    //  for (int j = 1000; j < 1016; j++) printf("%f ", data[i*w+j]);
    //  printf("\n");}


    //Free Host memory
    //printf("Free Host memory\n");
    //free(data);

    printf("\nDone\n");

    return 0;
}
