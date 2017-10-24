cmake_minimum_required(VERSION 3.8)
project(opencl_util)
include_directories("include")

include_directories(${OPENCL_HEADERS})

# Shared lib
find_library(OPENCL_LIB OpenCL PATHS ${CMAKE_CURRENT_BINARY_DIR})

add_library(opencl_util SHARED src/opencl_util.c)
target_link_libraries(opencl_util ${OPENCL_LIB})

# Example program oclinf
add_executable(oclinf src/oclinf.c)
target_link_libraries(oclinf ${OPENCL_LIB} opencl_util)

# allow 'for' loop initial declarations
set_property(TARGET opencl_util PROPERTY C_STANDARD 99)
