#include "opencl_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <search.h>
#include <stdarg.h>

#define asize(arr, type) (sizeof(arr) / sizeof(type))

typedef struct {
  cl_int value;
  const char *name;
} ocl_const_t;

ocl_const_t lu_cl_error_codes[] = {
  { CL_DEVICE_NOT_FOUND, "CL_DEVICE_NOT_FOUND" },
  { CL_DEVICE_NOT_AVAILABLE, "CL_DEVICE_NOT_AVAILABLE" },
  { CL_COMPILER_NOT_AVAILABLE, "CL_COMPILER_NOT_AVAILABLE" },
  { CL_MEM_OBJECT_ALLOCATION_FAILURE, "CL_MEM_OBJECT_ALLOCATION_FAILURE" },
  { CL_OUT_OF_RESOURCES, "CL_OUT_OF_RESOURCES" },
  { CL_OUT_OF_HOST_MEMORY, "CL_OUT_OF_HOST_MEMORY" },
  { CL_PROFILING_INFO_NOT_AVAILABLE, "CL_PROFILING_INFO_NOT_AVAILABLE" },
  { CL_MEM_COPY_OVERLAP, "CL_MEM_COPY_OVERLAP" },
  { CL_IMAGE_FORMAT_MISMATCH, "CL_IMAGE_FORMAT_MISMATCH" },
  { CL_IMAGE_FORMAT_NOT_SUPPORTED, "CL_IMAGE_FORMAT_NOT_SUPPORTED" },
  { CL_BUILD_PROGRAM_FAILURE, "CL_BUILD_PROGRAM_FAILURE" },
  { CL_MAP_FAILURE, "CL_MAP_FAILURE" },
  { CL_MISALIGNED_SUB_BUFFER_OFFSET, "CL_MISALIGNED_SUB_BUFFER_OFFSET" },
  { CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST, "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST" },
  { CL_COMPILE_PROGRAM_FAILURE, "CL_COMPILE_PROGRAM_FAILURE" },
  { CL_LINKER_NOT_AVAILABLE, "CL_LINKER_NOT_AVAILABLE" },
  { CL_LINK_PROGRAM_FAILURE, "CL_LINK_PROGRAM_FAILURE" },
  { CL_DEVICE_PARTITION_FAILED, "CL_DEVICE_PARTITION_FAILED" },
  { CL_KERNEL_ARG_INFO_NOT_AVAILABLE, "CL_KERNEL_ARG_INFO_NOT_AVAILABLE" },
  { CL_INVALID_VALUE, "CL_INVALID_VALUE" },
  { CL_INVALID_DEVICE_TYPE, "CL_INVALID_DEVICE_TYPE" },
  { CL_INVALID_PLATFORM, "CL_INVALID_PLATFORM" },
  { CL_INVALID_DEVICE, "CL_INVALID_DEVICE" },
  { CL_INVALID_CONTEXT, "CL_INVALID_CONTEXT" },
  { CL_INVALID_QUEUE_PROPERTIES, "CL_INVALID_QUEUE_PROPERTIES" },
  { CL_INVALID_COMMAND_QUEUE, "CL_INVALID_COMMAND_QUEUE" },
  { CL_INVALID_HOST_PTR, "CL_INVALID_HOST_PTR" },
  { CL_INVALID_MEM_OBJECT, "CL_INVALID_MEM_OBJECT" },
  { CL_INVALID_IMAGE_FORMAT_DESCRIPTOR, "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR" },
  { CL_INVALID_IMAGE_SIZE, "CL_INVALID_IMAGE_SIZE" },
  { CL_INVALID_SAMPLER, "CL_INVALID_SAMPLER" },
  { CL_INVALID_BINARY, "CL_INVALID_BINARY" },
  { CL_INVALID_BUILD_OPTIONS, "CL_INVALID_BUILD_OPTIONS" },
  { CL_INVALID_PROGRAM, "CL_INVALID_PROGRAM" },
  { CL_INVALID_PROGRAM_EXECUTABLE, "CL_INVALID_PROGRAM_EXECUTABLE" },
  { CL_INVALID_KERNEL_NAME, "CL_INVALID_KERNEL_NAME" },
  { CL_INVALID_KERNEL_DEFINITION, "CL_INVALID_KERNEL_DEFINITION" },
  { CL_INVALID_KERNEL, "CL_INVALID_KERNEL" },
  { CL_INVALID_ARG_INDEX, "CL_INVALID_ARG_INDEX" },
  { CL_INVALID_ARG_VALUE, "CL_INVALID_ARG_VALUE" },
  { CL_INVALID_ARG_SIZE, "CL_INVALID_ARG_SIZE" },
  { CL_INVALID_KERNEL_ARGS, "CL_INVALID_KERNEL_ARGS" },
  { CL_INVALID_WORK_DIMENSION, "CL_INVALID_WORK_DIMENSION" },
  { CL_INVALID_WORK_GROUP_SIZE, "CL_INVALID_WORK_GROUP_SIZE" },
  { CL_INVALID_WORK_ITEM_SIZE, "CL_INVALID_WORK_ITEM_SIZE" },
  { CL_INVALID_GLOBAL_OFFSET, "CL_INVALID_GLOBAL_OFFSET" },
  { CL_INVALID_EVENT_WAIT_LIST, "CL_INVALID_EVENT_WAIT_LIST" },
  { CL_INVALID_EVENT, "CL_INVALID_EVENT" },
  { CL_INVALID_OPERATION, "CL_INVALID_OPERATION" },
  { CL_INVALID_GL_OBJECT, "CL_INVALID_GL_OBJECT" },
  { CL_INVALID_BUFFER_SIZE, "CL_INVALID_BUFFER_SIZE" },
  { CL_INVALID_MIP_LEVEL, "CL_INVALID_MIP_LEVEL" },
  { CL_INVALID_GLOBAL_WORK_SIZE, "CL_INVALID_GLOBAL_WORK_SIZE" },
  { CL_INVALID_PROPERTY, "CL_INVALID_PROPERTY" },
  { CL_INVALID_IMAGE_DESCRIPTOR, "CL_INVALID_IMAGE_DESCRIPTOR" },
  { CL_INVALID_COMPILER_OPTIONS, "CL_INVALID_COMPILER_OPTIONS" },
  { CL_INVALID_LINKER_OPTIONS, "CL_INVALID_LINKER_OPTIONS" },
  { CL_INVALID_DEVICE_PARTITION_COUNT, "CL_INVALID_DEVICE_PARTITION_COUNT" },
};

ocl_const_t lu_cl_platform_info[] = {
  { CL_PLATFORM_PROFILE, "CL_PLATFORM_PROFILE" },
  { CL_PLATFORM_VERSION, "CL_PLATFORM_VERSION" },
  { CL_PLATFORM_NAME, "CL_PLATFORM_NAME" },
  { CL_PLATFORM_VENDOR, "CL_PLATFORM_VENDOR" },
  { CL_PLATFORM_EXTENSIONS, "CL_PLATFORM_EXTENSIONS" }
};

static void err_common(cl_int err, const char *msg, va_list ap)
{
  size_t count = sizeof(lu_cl_error_codes) / sizeof(ocl_const_t);
  const char *name;

  for (size_t i = 0; i < count; ++i)
  {
    if (lu_cl_error_codes[i].value == err)
    {
      name = lu_cl_error_codes[i].name;
      break;
    }
  }

  printf("Error(%s): ", name);
  vprintf(msg, ap);
}

void err_warn(cl_int err, const char *msg, ...)
{
  if (err != CL_SUCCESS)
  {
    va_list ap;
    va_start(ap, msg);
    err_common(err, msg, ap);
    va_end(ap);
  }
}

void err_exit(cl_int err, const char *msg, ...)
{
  if (err != CL_SUCCESS)
  {
    va_list ap;
    va_start(ap, msg);
    err_common(err, msg, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
  }
}

void info_dump()
{
  cl_int err;
  cl_platform_id *platform_ids = NULL;
  cl_uint platform_ids_size;
  size_t i, j;
  char *char_ptr = NULL;
  size_t char_ptr_size;

  err = clGetPlatformIDs(0, NULL, &platform_ids_size);
  err_exit(err, "Failed clGetPlatformIDs");

  platform_ids = (cl_platform_id *)malloc(platform_ids_size *
      sizeof(cl_platform_id));

  err = clGetPlatformIDs(platform_ids_size, platform_ids, NULL);

  printf("Found %u OpenCL platforms.\n", platform_ids_size);

  for (i = 0; i < platform_ids_size; ++i)
  {
    printf("Platform[%zu]:\n", i);

    for (j = 0; j < asize(lu_cl_platform_info, ocl_const_t); ++j)
    {
      ocl_const_t tmp = lu_cl_platform_info[j];

      err = clGetPlatformInfo(platform_ids[i], tmp.value, 0, NULL,
          &char_ptr_size);
      err_exit(err, "Failed clGetPlatformInfo(%s)\n", tmp.name);

      char_ptr = (char *)realloc(char_ptr, char_ptr_size * sizeof(char));

      err = clGetPlatformInfo(platform_ids[i], tmp.value, char_ptr_size,
          char_ptr, NULL);
      err_exit(err, "Failed clGetPlatformInfo(%s)\n", tmp.name);

      printf("%s : %s\n", tmp.name, char_ptr);
    }
  }

  free(char_ptr);
  free(platform_ids);
}
