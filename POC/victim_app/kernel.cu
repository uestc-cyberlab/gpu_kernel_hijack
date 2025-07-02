#include <cuda.h>
#include <stdlib.h>
#include <stdio.h>

// Device code
extern "C" __global__ void kernel(unsigned char* A, unsigned char* B, int n) 
{
    	int idx = blockDim.x * blockIdx.x + threadIdx.x;
	int threshold = 230;
    	if(idx<n)
    	{
    		if(B[idx]>=threshold)
    		{
    			A[idx] = 255;
    		}
    		else
    		{
    			A[idx] = 0;
    		}
    	}
}

//nvcc -arch=sm_61 --cubin kernel.cu

