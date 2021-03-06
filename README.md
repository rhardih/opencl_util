# opencl_util

Various utility functions to ease working with OpenCL.

## Documentation

See [opencl_util.h](https://github.com/rhardih/opencl_util/blob/master/include/opencl_util.h) for a list of available functions.


## Examples

### oclinf

There's a small example program included, which just calls `ocu_info_dump()` and
then exits. It should be present in the build folder after a successful `make`.

Example output abbreviated:

```bash
$ ./oclinf
Found 1 OpenCL platforms.
Platform[0]:
CL_PLATFORM_PROFILE : FULL_PROFILE
CL_PLATFORM_VERSION : OpenCL 1.2 (May 26 2017 12:59:48)
CL_PLATFORM_NAME : Apple
CL_PLATFORM_VENDOR : Apple
CL_PLATFORM_EXTENSIONS : cl_APPLE_SetMemObjectDestructor
cl_APPLE_ContextLoggingFunctions cl_APPLE_clut cl_APPLE_query_kernel_names
cl_APPLE_gl_sharing cl_khr_gl_event
Found 2 devices for Platform[0]
Device[0]:
CL_DEVICE_TYPE: { CL_DEVICE_TYPE_CPU }
CL_DEVICE_VENDOR_ID: 4294967295
CL_DEVICE_MAX_COMPUTE_UNITS: 8
...
```


### Library functions

Select outputs from some of the info functions for reference:

#### ocu_image_info

Given `img` is an already initialized instance of `cl_mem`:

Code:

```c
ocu_image_info(img);
```

Output:

```
Image info for cl_mem 0x7fb03f9443a0 :
CL_IMAGE_FORMAT: {
  image_channel_order: CL_RGBA
  image_channel_data_type: CL_UNORM_INT8
}
CL_IMAGE_ELEMENT_SIZE: 4
CL_IMAGE_ROW_PITCH: 7680
CL_IMAGE_SLICE_PITCH: 0
CL_IMAGE_WIDTH: 1920
CL_IMAGE_HEIGHT: 1080
CL_IMAGE_DEPTH: 0
CL_IMAGE_ARRAY_SIZE: 0
CL_IMAGE_BUFFER: 0x7fb03f9448b0
CL_IMAGE_NUM_MIP_LEVELS: 0
CL_IMAGE_NUM_SAMPLES: 0
```

#### ocu_queue_info

Given `queue` is an already initialized instance of `cl_command_queue`:

Code:

```c
ocu_image_info(queue);
```

Output:

```
Queue info for cl_command_queue 0x7fb0386ec8c0 :
CL_QUEUE_CONTEXT: 0x7fb03f9448b0
CL_QUEUE_DEVICE: 0x7fb03f9448b0
CL_QUEUE_REFERENCE_COUNT: 1
CL_QUEUE_PROPERTIES: { CL_QUEUE_PROFILING_ENABLE }
```

## Building

This library uses CMake to generate build configurations. Please see
[https://cmake.org/install/](https://cmake.org/install/) for system specific
install instructions.

### macOS

Since OpenCL is already present on most macOS systems, building on this system
should be as simple as:

```bash
mkdir build
cd build
cmake ..
make
```

You should now see a `libopencl_util.dylib` file in the build folder:

```bash
$ file libopencl_util.dylib
libopencl_util.dylib: Mach-O 64-bit dynamically linked shared library x86_64
```

### Android

Building for Android is a little bit more involved, since you'll need both a
working Android NDK, the OpenCL header files, and an architecture specific copy
of `libOpenCL.so` for the linker step. See subsections below for instructions on
how to obtain each of these.

In this example the Android NDK is located at `~/Code/Android/android-ndk-r10e/`
and `OpenCL-Headers` is checked out at `~/Code/OpenCL-Headers/`

Assuming you got all the requirements sorted, build it like so:

```bash
export OPENCL_HEADERS=~/Code/OpenCL-Headers/opencl12/
export ANDROID_NDK_ROOT=~/Code/Android/android-ndk-r10e/
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../android.toolchain.cmake ..
make
```

You should now see a `libopencl_util.so` file in the build folder:

```bash
$ file libopencl_util.so
libopencl_util.so: ELF 32-bit LSB shared object, ARM, EABI5 version 1 (SYSV), dynamically linked, interpreter /system/bin/linker, not stripped
```

#### NDK

You can download a copy of the Android NDK at:

[https://developer.android.com/ndk/downloads/](https://developer.android.com/ndk/downloads/)

Beware, only `android-ndk-r10e` have been tested at the moment.

#### OpenCL-Headers

Clone the official KhronosGroup headers repo:

```bash
git clone https://github.com/KhronosGroup/OpenCL-Headers.git
```

At the time of writing, this includes headers for versions 1.0 through 2.2 of the spec.

#### libOpenCL.so

The easiest way to acquire a copy of this library, is to pull it from an actual
phone. The location of the library possibly varies from phone to phone, so you
might have to dig around to find it. You can do so, by connecting with `adb` and
searching. Eg.:

```bash
$ adb shell
:/ $ find /system/vendor/lib -name *OpenCL*
/system/vendor/lib/libOpenCL.so
```

In this case it's located in the `/system/vendor/lib` folder. Copy it from the
phone like so:

```bash
$ adb pull /system/vendor/lib/libOpenCL.so .
/system/vendor/lib/libOpenCL.so: 1 file pulled. 6.9 MB/s (75900 bytes in 0.010s)
```
From here, there's two places the library can be moved to.

1. When run CMake will look for the OpenCL library in the current build folder,
   so moving it there should work just fine:

   ```bash
   mv libOpenCL.so build/
   ```

2. Alternatively copying the library directly to the specific NDK platforms
   `lib` directory. In this case `android-21` is used, so that would be:

   ```bash
   cp libOpenCL.so ~/Code/Android/android-ndk-r10e/platforms/android-21/arch-arm/usr/lib/
   ```

With 1. you avoid "polluting" your toolchain, but with 2. you can reuse it for
other lookups.

## License

MIT - [https://rhardih.mit-license.org](https://rhardih.mit-license.org/)
