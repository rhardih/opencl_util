cmake_minimum_required(VERSION 3.8)
project(opencl_util)
include_directories("include")
include_directories(${OPENCL_HEADERS})
add_library(opencl_util SHARED src/opencl_util.c)
find_library(OPENCL_LIB OpenCL PATHS ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(opencl_util ${OPENCL_LIB})

# allow 'for' loop initial declarations
set_property(TARGET opencl_util PROPERTY C_STANDARD 99)
