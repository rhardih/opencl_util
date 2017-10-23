CC = gcc
CFLAGS = -std=c99

all: lib

debug: CFLAGS += -g
debug: lib

lib: opencl_util.o out_dirs
	$(CC) $(CFLAGS) -Wall -shared -framework OpenCL -o lib/libopencl_util.so opencl_util.o

opencl_util.o: include/opencl_util.h src/opencl_util.c
	$(CC) $(CFLAGS) -fPIC -c -Iinclude src/opencl_util.c

oclinf: src/oclinf.c out_dirs
	$(CC) -Llib -lopencl_util -Iinclude -o bin/oclinf src/oclinf.c

out_dirs:
	mkdir -p lib bin

clean:
	rm *.o lib/libopencl_util.so
