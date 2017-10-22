CC = gcc

all: lib

debug: CFLAGS += -g
debug: lib

lib: opencl_util.o lib_dir
	$(CC) $(CFLAGS) -Wall -shared -framework OpenCL -o lib/libopencl_util.so opencl_util.o

opencl_util.o: include/opencl_util.h src/opencl_util.c
	$(CC) $(CFLAGS) -fPIC -c -Iinclude src/opencl_util.c

lib_dir:
	mkdir -p lib

clean:
	rm *.o lib/libopencl_util.so
