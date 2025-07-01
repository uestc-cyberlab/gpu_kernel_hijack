# <center><font size=5> A Kernel Hijacking Vulnerability on NVIDIA GPUs</font></center>

We found a kernel hijacking vulnerability on NVIDIA GPUs. Attackers can manipulate the invocation mechanism of the CUDA driver APIs, and replace the kernel loading API by a malicious one, which can be exploited to launch a GPU kernel hijacking attack covertly. The remainder of this paper is organized as follows. Section I introduces the basics of GPU programming model. Section II details the GPU kernel hijacking method. Section III presents an interesting attack case.



## <font size=5> I. Introduction</font>

In a GPU-based parallel computing platform, the GPU serves as a co-processor (device) of a CPU (host). On the one hand, a host needs to use APIs provided by the manufacturer to manage a GPU, such as sending the original data, launching the target GPU and receiving the final results. On the other hand, the GPU executes a specific kernel function to realize data processing acceleration.

**Device side**: as shown in the following figure, a GPU kernel is a specific function executed on the target GPU, which implements two vector additions in parallel. A kernel is usually programmed in C/C++ language. We need to use the NVIDIA compiler NVCC to compile the kernel source code into a set of executable machine code instructions. Specifically, it is first compiled into PTX (parallel thread execution) code which is an intermediate-level and virtual instruction set. Then, the PTX code will be further compiled into a set of SASS instructions which can be directly executed on GPU. Finally, these instructions are packaged in an executable binary file (.cubin file).

**Host side**: a host uses a group of driver APIs to manage the above kernel. It typically involves three steps: inputting the original data, invoking the kernel to process data and receiving the final results. First, since a GPU has its independent memory system, a host requires to allocate data space on it and then copy input data from host memory into the device memory (line 5-10). Then, the host manages and launches the kernel to process input data on the GPU (line 11-20). Specifically, the host first creates a context to maintain the state of the target GPU (line 13-14). Then, according to the kernel file's path, the host loads the above kernel into a module(line 15-16), and extracts the function from the module by kernel's name (line 17-18). Finally, the host configures and launches the kernel to process data with a certain computing resource, such as the number of threads, size of shared memory, etc. (line 19-20). 

![GPU1](https://github.com/uestc-cyberlab/gpu_kernel_hijack/blob/main/images/GPU%20programming%20and%20executing%20model.png)



## <font size=5> III. Attack Method</font>



![GPU2](https://github.com/uestc-cyberlab/gpu_kernel_hijack/blob/main/images/hijack.png)








## <font size=5> III. Attack Case </font>






<!-- cd /D/github/gpu_kernel_hijack -->
<!-- git add . -->
<!-- git commit -m "Initial commit" -->
<!-- git push origin main -->

<!-- ctrl+k v -->

