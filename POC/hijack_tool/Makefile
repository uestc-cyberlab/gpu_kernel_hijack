image_hijack.so: hijack.o
	nvcc -arch=sm_61 hijack.o -lcuda -lcudart_static -shared -o image_hijack.so
	
hijack.o: hijack.cu
	nvcc -Xptxas -astoolspatch --keep-device-functions -arch=sm_61 -Xcompiler -fPIC -c hijack.cu -o hijack.o

clean:
	rm -f *.so *.o
