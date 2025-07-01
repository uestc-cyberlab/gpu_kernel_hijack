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






<!-- cd /D/github/gpu_kernel_hijack -->
<!-- git add . -->
<!-- git commit -m "Initial commit" -->
<!-- git push origin main -->

<!-- ctrl+k v -->

