image_segment.o: image_segment.cpp kernel.cubin
	nvcc -arch=sm_61 -lcuda `pkg-config --cflags --libs opencv` image_segment.cpp -o image_segment.o


kernel.cubin: kernel.cu
	nvcc -arch=sm_61 --cubin kernel.cu


clean:
	rm -f *.o *cubin
