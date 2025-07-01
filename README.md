# <center><font size=5> A Kernel Hijacking Vulnerability on NVIDIA GPUs</font></center>

We found a kernel hijacking vulnerability on NVIDIA GPUs. Attackers can manipulate the invocation mechanism of the CUDA driver APIs, and replace the kernel loading API by a malicious one, which can be exploited to launch a GPU kernel hijacking attack covertly. The remainder of this paper is organized as follows. Section I introduces the basics of GPU programming model. Section II details the GPU kernel hijacking method. Section III presents an interesting attack case.



## <font size=5> I. Introduction</font>

In a GPU-based parallel computing platform, the GPU serves as a co-processor (device) of a CPU (host). On the one hand, a host needs to use APIs provided by the manufacturer to manage a GPU, such as sending the original data, launching the target GPU and receiving the final results. On the other hand, the GPU executes a specific kernel function to realize data processing acceleration.

**Device side**: as shown in the right of Fig. 1, a GPU kernel is a specific function executed on the target GPU, which implements two vector additions in parallel. A kernel is usually programmed in C/C++ language. We need to use the NVIDIA compiler NVCC to compile the kernel source code into a set of executable machine code instructions. Specifically, it is first compiled into PTX (parallel thread execution) code which is an intermediate-level and virtual instruction set. Then, the PTX code will be further compiled into a set of SASS instructions which can be directly executed on GPU. Finally, these instructions are packaged in an executable binary file (.cubin file).

**Host side**: as shown in the left of Fig. 1, a host uses a group of driver APIs to manage the above kernel. It typically involves three steps: inputting the original data, invoking the kernel to process data and receiving the final results. First, since a GPU has its independent memory system, a host requires to allocate data space on it and then copy input data from host memory into the device memory (line 5-10). Then, the host manages and launches the kernel to process input data on the GPU (line 11-20). Specifically, the host first creates a context to maintain the state of the target GPU (line 13-14). Then, according to the kernel file's path, the host loads the above kernel into a module(line 15-16), and extracts the function from the module by kernel's name (line 17-18). Finally, the host configures and launches the kernel to process data with a certain computing resource, such as the number of threads, size of shared memory, etc. (line 19-20). 

![GPU1](https://github.com/uestc-cyberlab/gpu_kernel_hijack/blob/main/images/programming_model.png)



## <font size=5> III. Attack Method</font>

In this section, we first analyze the invocation mechanism of driver API and then detail how to manipulate a malicious API to hijack GPU kernel.

As shown in Fig. 2 (a), a CUDA program contains two parts, device code and host code. The device code is compiled into an executable kernel file by NVCC. Meanwhile, the host code is compiled by G++ and executed in the host memory space. When a host program needs to invoke a group of APIs to manage the GPU kernel, their DLLs will be linked to the target program by the linker and executed in host memory. Specifically, when a host requires to call the **cuModuleLoad** function to load a kernel, the operating system (like Linux) first uses the dynamic linker **ld.so** to load its DLL (dynamic link library) from the CUDA APIs library into host memory. Next, this DLL will be linked to the target program and executed to load the kernel into a computing module. Finally, we conclude that the API invocation mechanism has the characteristics of dynamic loading and sharing, which can protect intellectual property and save memory usage.

![GPU2](https://github.com/uestc-cyberlab/gpu_kernel_hijack/blob/main/images/hijack.png)

Unfortunately, the API invocation mechanism exists security risks. The dynamic linker can be manipulated by an adversary to implement kernel hijacking.
In detail, the linker only inspects the interface of a API and does not have other verification strategies. Moreover, it also provides a preloading mechanism. A specific API can be linked to the target program with a higher priority. Therefore, an adversary can craft a disguised kernel loading API to replace the normal one and thus hijack the GPU kernel. To achieve this goal, the following two conditions must be met.

**(1) The same application binary interface.** The API's DLL is a binary file, and the dynamic linker only checks its interface, such as function name, number and type of arguments. Therefore, an attacker can construct a malicious API that maintains the same interface as the original one. In the malicious API, an attacker can implement arbitrary logic.

**(2) The higher linking priority.** To replace the normal API invocation with a malicious one, it is necessary to raise the linking priority of the malicious DLL. Modern operating systems, like Linux, provide a DLL preloading mechanism, which can escalate the linking priority of a DLL. Hence, the victim GPU application could wrongly prioritize calling the malicious API.

Finally, an attacker can hijack a GPU kernel through the above two steps. We conclude the principle of our kernel hijacking method in Fig. 2 (b). The attacker first elaborately designs a malicious API/DLL and modifies the load sequence of the dynamic linker. Second, when a victim GPU application is required to invoke a specific API, the linker will preload the malicious DLL to replace the normal one. Third, once the malicious DLL is executed, it will hijack the GPU kernel and inject malicious logic into it. Finally, the Trojan-infected kernel is loaded into GPU and launched to implement illegal purposes.




## <font size=5> III. Attack Case </font>

Image processing is one of the most important GPU applications. GPU can calculate a large number of pixels simultaneously, which greatly accelerates image processing. In this subsection, we implement a medical image segmentation application on GPU and show how to tamper with the segmentation result.

**Experimental Setup**

| Operating System |              GPU Model               | CUDA Version |
| :--------------: | :----------------------------------: | :----------: |
|  Ubuntu 2022.04  | NVIDIA GTX1050 (Pascal Arch., SM_61) |     12.2     |

**Victim**

As shown in Fig. 3 (a), it is a brain magnetic resonance imaging. Compared to the normal area, the gray value of the tumor region is significantly higher. Hence, the medical personnel can utilize a threshold segmentation algorithm to highlight the affected brain area. If the pixel value is greater than the threshold, it will be reassigned to a new gray value, like 255 (white). Otherwise, it will be assigned to 0 (black). After all the pixels are processed, the medical personnel can acquire the detected tumor region. 

```c++
__global__ void kernel(unsigned char I_image, //input image
                       unsigned char O_image) //output image
{
	int i=blockDim.x*blockIdx.x+threadIdx.x;
    int threshold = 230;
    /* threshold segmentation */
	if (I_image[i]>threshold)
		O_image[i]=255;
	else
		O_image[i]=0;
}
```
Finally, the victim compiles and executes this kernel. In Fig. 3 (b), we show the processed image, where the white region is the detected tumor. We can observe that the tumor area is large, which indicates the patient's condition is very serious. The entire program is located in **/ImageProcess/segment_gpu**. 

```
./image_segment.o
```

![GPU3](https://github.com/uestc-cyberlab/gpu_kernel_hijack/blob/main/images/reference.png)

**Attacker**

An attacker designs a malicious kernel loading API and compiles it to a DLL, as introduced in Section II. An attacker implements a malicious kernel loading API which has the same interface with the normal one. In the malicious API, attacker illegally modifies the GPU kernel file. This entire program is located in **/ImageProcess/hijack_tool/hijack.cu**. 

```c++
CUresult CUDAAPI cuModuleLoad(CUmodule *module, const char *fname)
{
	//1.inject malicous instruction into cubin
	char* image = injector(fname);
	//2.load malicious cubin image into cuModule
	CUresult code = cuModuleLoadData(module, image);
	if(code!=CUDA_SUCCESS)
	{
		return CUDA_ERROR_INVALID_VALUE;
	}
	return CUDA_SUCCESS;
}
```

After that, attacker compiles the malicious kernel loading API to a dynamic link library (DLL). The compile command is located in **/ImageProcess/hijack_tool/Makefile**. 

```makefile
image_hijack.so: hijack.o
	nvcc -arch=sm_61 hijack.o -lcuda -lcudart_static -shared -o image_hijack.so
	
hijack.o: hijack.cu
	nvcc -Xptxas -astoolspatch --keep-device-functions -arch=sm_61 -Xcompiler -fPIC -c hijack.cu -o hijack.o

clean:
	rm -f *.so *.o
```

Next, the attacker modifies the pre-loading environment variable "LD_PRELOAD" or the pre-loading  configuration file "/etc.ld.so.preload" to escalate the linking priority of the malicious DLL.

```
LD_PRELOAD = ./image_hijack.so
```

```
sudo vim /etc.ld.so.preload
./image_hijack.so
```

Finally, once the victim launch the CUDA program, the malicious API will hijack the GPU kernel and implant a Trojan. As shown in Fig. 4, it will make the detected tumor area decrease sharply. We can observe that the tumor area is very small, which may cause a misjudgment of the patient's condition. This can result in a serious medical accident. 

![GPU4](https://github.com/uestc-cyberlab/gpu_kernel_hijack/blob/main/images/malicious.png)

