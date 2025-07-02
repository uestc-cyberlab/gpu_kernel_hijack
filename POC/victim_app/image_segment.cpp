#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <cuda.h>
#include <cuda_runtime.h>


using namespace cv;
using namespace std;

int main()
{
	//1.read image
	Mat image_r = imread("tumor.jpg", IMREAD_GRAYSCALE); //read origional image
	//imshow("image_raw", image_r);
	int row =image_r.rows;
	int col =image_r.cols;
	int size = row*col*sizeof(unsigned char);
	//int chan = image_r.channels(); //grayscale image's channel is 1
	//cout<<"resolution is "<<col<<"* "<<row<<endl;
	//cout<<"channel is "<<chan<<endl;
	
	
	//2.process image
	//2.1.image filtering
	Mat image_f;
	medianBlur(image_r, image_f, 5); //median filtering
	//imshow("image_f", image_f);
	//2.2.copy image into array
	unsigned char* h_B = (unsigned char*)malloc(size);
	for(int i=0; i<row; i++)
	{
		unsigned char* list = image_f.ptr<unsigned char>(i); //one row in image
		for(int j=0; j<col; j++)
		{
			h_B[i*row+j] = list[j];
		}
	}
	
	
	
	
	//2.3.threshold segmentation
	cuInit(0);
	
	CUdevice cuDevice;
	cuDeviceGet(&cuDevice, 0);
	
	CUcontext cuContext;
	cuCtxCreate(&cuContext, 0, cuDevice);
	
	CUmodule cuModule;
	cuModuleLoad(&cuModule, "/home/zy/Desktop/ImageProcess/segment_gpu/kernel.cubin");
	
	CUdeviceptr d_A;
	cuMemAlloc(&d_A, size);
	
	CUdeviceptr d_B;
	cuMemAlloc(&d_B, size);
	cuMemcpyHtoD(d_B, h_B, size);
	
	CUfunction kernel;
	cuModuleGetFunction(&kernel, cuModule, "kernel");
	
	int n = row * col;
	int threadsPerBlock = 256;
	int blocksPerGrid = (n+threadsPerBlock-1)/threadsPerBlock;
	void* args[] = {&d_A, &d_B, &n};
	cuLaunchKernel(kernel, blocksPerGrid, 1, 1, threadsPerBlock, 1, 1, 0, 0, args, 0);
	cudaError_t err = cudaGetLastError();
	printf("%s \n", cudaGetErrorString(err));
	
	unsigned char* h_A = (unsigned char*)malloc(size);
	cuMemcpyDtoH(h_A, d_A, size);
	
	
	
	
	//3.save image
	Mat image_p(row, col, CV_8U, 255); //create new image, CV_8U is data type, 255 is initialized value
	for(int i=0; i<row; i++) //copy array into image
	{
		unsigned char* list = image_p.ptr<unsigned char>(i);
		for(int j=0; j<col; j++)
		{
			list[j] = h_A[i*row+j];
		}
	}
	//imshow("image_processed", image_p);
	imwrite("image_p.jpg", image_p); //save image

	
	//waitKey(0); //pause terminal to show image
	free(h_A);
	free(h_B);
	cuMemFree(d_A);
	cuMemFree(d_B);
	cuCtxDestroy(cuContext);
	return 0;
}


//nvcc -arch=sm_61 -lcuda `pkg-config --cflags --libs opencv` image_segment.cu -o image_segment.o

