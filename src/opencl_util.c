#include "opencl_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#define asize(arr, type) (sizeof(arr) / sizeof(type))

typedef struct {
  cl_int value;
  const char *name;
} ocl_const_t;

typedef struct {
  cl_uint value;
  const char *name;
  void (*printer)();
} ocl_const_uint_t;

typedef struct {
  cl_ulong value;
  const char *name;
} ocl_const_ulong_t;

typedef struct {
  cl_device_partition_property value;
  const char *name;
} ocl_const_cl_device_partition_property;

static const ocl_const_t lu_cl_error_codes[] = {
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

static const ocl_const_uint_t lu_cl_platform_info[] = {
  { CL_PLATFORM_PROFILE, "CL_PLATFORM_PROFILE" },
  { CL_PLATFORM_VERSION, "CL_PLATFORM_VERSION" },
  { CL_PLATFORM_NAME, "CL_PLATFORM_NAME" },
  { CL_PLATFORM_VENDOR, "CL_PLATFORM_VENDOR" },
  { CL_PLATFORM_EXTENSIONS, "CL_PLATFORM_EXTENSIONS" }
};

// Type related printer functions
static void pr_cl_bitfield(cl_bitfield value, const ocl_const_ulong_t lookup[],
    size_t lookup_size)
{
  printf("{ ");

  size_t i, j;

  for (i = 0, j = 0; i < lookup_size; ++i)
  {
    if ((value & lookup[i].value) == lookup[i].value)
    {
      if (j++ != 0)
        printf(", ");
      printf("%s", lookup[i].name);
    }
  }

  printf(" }\n");
}

static const ocl_const_ulong_t lu_cl_device_types[] = {
  { CL_DEVICE_TYPE_DEFAULT, "CL_DEVICE_TYPE_DEFAULT" },
  { CL_DEVICE_TYPE_CPU, "CL_DEVICE_TYPE_CPU" },
  { CL_DEVICE_TYPE_GPU, "CL_DEVICE_TYPE_GPU" },
  { CL_DEVICE_TYPE_ACCELERATOR, "CL_DEVICE_TYPE_ACCELERATOR" },
  { CL_DEVICE_TYPE_CUSTOM, "CL_DEVICE_TYPE_CUSTOM" },
  { CL_DEVICE_TYPE_ALL, "CL_DEVICE_TYPE_ALL" },
};

static void pr_cl_device_type(const char *name, void *value_ptr)
{
  cl_bitfield value = *(cl_bitfield *)value_ptr;

  printf("%s: ", name);
  pr_cl_bitfield(value, lu_cl_device_types, asize(lu_cl_device_types,
        ocl_const_ulong_t));
}

static void pr_cl_ulong(const char *name, void *value_ptr)
{
  cl_ulong value = *(cl_ulong *)value_ptr;

  printf("%s: %llu\n", name, value);
}

static void pr_cl_uint(const char *name, void *value_ptr)
{
  cl_uint value = *(cl_ulong *)value_ptr;

  printf("%s: %u\n", name, value);
}

static void pr_size_t(const char *name, void *value_ptr)
{
  size_t value = *(size_t *)value_ptr;

  printf("%s: %zu\n", name, value);
}

static void pr_size_t_arr(const char *name, void *value_ptr, size_t bytes)
{
  size_t *value = (size_t *)value_ptr;

  printf("%s: { ", name);

  for (size_t i = 0; i < (bytes / sizeof(size_t)); ++i)
  {
    if (i != 0)
      printf(", ");
    printf("%zu", value[i]);
  }

  printf(" }\n");
}

static void pr_cl_bool(const char *name, void *value_ptr)
{
  cl_bool value = *(cl_bool *)value_ptr;

  if (value)
    printf("%s: CL_TRUE\n", name);
  else
    printf("%s: CL_FALSE\n", name);
}

static void pr_char_arr(const char *name, void *value_ptr)
{
  char *value = (char *)value_ptr;

  printf("%s: %s\n", name, value);
}

static void pr_cl_platform_id(const char *name, void *value_ptr)
{
  cl_platform_id value = *(cl_platform_id *)value_ptr;

  printf("%s: %p\n", name, value);
}

static void pr_cl_device_id(const char *name, void *value_ptr)
{
  printf("%s: ", name);
  if (value_ptr == NULL)
  {
    printf("NULL");
  }
  else
  {
    cl_device_id value = *(cl_device_id *)value_ptr;
    printf("%p\n", value);
  }
}

static const ocl_const_ulong_t lu_cl_device_fp_config[] = {
  { CL_FP_DENORM, "CL_FP_DENORM" },
  { CL_FP_INF_NAN, "CL_FP_INF_NAN" },
  { CL_FP_ROUND_TO_NEAREST, "CL_FP_ROUND_TO_NEAREST" },
  { CL_FP_ROUND_TO_ZERO, "CL_FP_ROUND_TO_ZERO" },
  { CL_FP_ROUND_TO_INF, "CL_FP_ROUND_TO_INF" },
  { CL_FP_FMA, "CL_FP_FMA" },
  { CL_FP_SOFT_FLOAT, "CL_FP_SOFT_FLOAT" },
  { CL_FP_CORRECTLY_ROUNDED_DIVIDE_SQRT, "CL_FP_CORRECTLY_ROUNDED_DIVIDE_SQRT" },
};

static void pr_cl_device_fp_config(const char *name, void *value_ptr)
{
  cl_bitfield value = *(cl_bitfield *)value_ptr;

  printf("%s: ", name);
  pr_cl_bitfield(value, lu_cl_device_fp_config, asize(lu_cl_device_fp_config,
        ocl_const_uint_t));
}

static void pr_cl_uint_lookup(cl_uint value, const ocl_const_uint_t lookup[],
    size_t lookup_size)
{
  for (size_t i = 0; i < lookup_size; ++i)
  {
    if (lookup[i].value == value)
    {
      printf("%s\n", lookup[i].name);
      break;
    }
  }
}

static const ocl_const_uint_t lu_cl_device_mem_cache_type[] = {
  { CL_NONE, "CL_NONE" },
  { CL_READ_ONLY_CACHE, "CL_READ_ONLY_CACHE" },
  { CL_READ_WRITE_CACHE, "CL_READ_WRITE_CACHE" },
};

static void pr_cl_device_mem_cache_type(const char *name, void *value_ptr)
{
  cl_device_mem_cache_type value = *(cl_device_mem_cache_type *)value_ptr;

  printf("%s: ", name);
  pr_cl_uint_lookup(value,
      lu_cl_device_mem_cache_type,
      asize(lu_cl_device_mem_cache_type, ocl_const_uint_t));
}

static const ocl_const_uint_t lu_cl_device_local_mem_type[] = {
  { CL_NONE, "CL_NONE" },
  { CL_LOCAL, "CL_LOCAL" },
  { CL_GLOBAL, "CL_GLOBAL" },
};

static void pr_cl_device_local_mem_type(const char *name, void *value_ptr)
{
  cl_device_local_mem_type value = *(cl_device_local_mem_type *)value_ptr;

  printf("%s: ", name);
  pr_cl_uint_lookup(value,
      lu_cl_device_local_mem_type,
      asize(lu_cl_device_local_mem_type, ocl_const_uint_t));
}


static const ocl_const_ulong_t lu_cl_device_exec_capabilities[] = {
  { CL_EXEC_KERNEL, "CL_EXEC_KERNEL" },
  { CL_EXEC_NATIVE_KERNEL, "CL_EXEC_NATIVE_KERNEL" },
};

static void pr_cl_device_exec_capabilities(const char *name, void *value_ptr)
{
  cl_bitfield value = *(cl_bitfield *)value_ptr;

  printf("%s: ", name);
  pr_cl_bitfield(value,
      lu_cl_device_exec_capabilities,
      asize(lu_cl_device_exec_capabilities, ocl_const_ulong_t));
}

static const ocl_const_ulong_t lu_cl_command_queue_properties[] = {
  { CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, "CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE" },
  { CL_QUEUE_PROFILING_ENABLE, "CL_QUEUE_PROFILING_ENABLE" },
};

static void pr_cl_command_queue_properties(const char *name, void *value_ptr)
{
  cl_bitfield value = *(cl_bitfield *)value_ptr;

  printf("%s: ", name);
  pr_cl_bitfield(value,
      lu_cl_command_queue_properties,
      asize(lu_cl_command_queue_properties, ocl_const_ulong_t));
}

static const ocl_const_cl_device_partition_property lu_cl_device_partition_property[] = {
  { CL_DEVICE_PARTITION_EQUALLY, "CL_DEVICE_PARTITION_EQUALLY" },
  { CL_DEVICE_PARTITION_BY_COUNTS, "CL_DEVICE_PARTITION_BY_COUNTS" },
  { CL_DEVICE_PARTITION_BY_COUNTS_LIST_END, "CL_DEVICE_PARTITION_BY_COUNTS_LIST_END" },
  { CL_DEVICE_PARTITION_BY_AFFINITY_DOMAIN, "CL_DEVICE_PARTITION_BY_AFFINITY_DOMAIN" },
};

static void pr_cl_device_partition_property(const char *name, void *value_ptr,
    size_t bytes)
{
  cl_device_partition_property *value = value_ptr;

  printf("%s: ", name);

  if (value[0] == 0)
  {
    printf("Not supported\n");
  }
  else
  {
    printf("{ ");

    size_t i, j, k;
    size_t props_size = asize(lu_cl_device_partition_property,
        cl_device_partition_property);

    for (i = 0, k = 0; i < (bytes / sizeof(cl_device_partition_property)); ++i)
    {
      for (j = 0; j < props_size; ++j)
      {
        if (value[i] == lu_cl_device_partition_property[j].value)
        {
          if (k++ != 0)
            printf(", ");
          printf("%s", lu_cl_device_partition_property[j].name);
        }
      }
    }

    printf(" }\n");
  }
}

static const ocl_const_ulong_t lu_cl_device_affinity_domain[] = {
  { CL_DEVICE_AFFINITY_DOMAIN_NUMA, "CL_DEVICE_AFFINITY_DOMAIN_NUMA" },
  { CL_DEVICE_AFFINITY_DOMAIN_L4_CACHE, "CL_DEVICE_AFFINITY_DOMAIN_L4_CACHE" },
  { CL_DEVICE_AFFINITY_DOMAIN_L3_CACHE, "CL_DEVICE_AFFINITY_DOMAIN_L3_CACHE" },
  { CL_DEVICE_AFFINITY_DOMAIN_L2_CACHE, "CL_DEVICE_AFFINITY_DOMAIN_L2_CACHE" },
  { CL_DEVICE_AFFINITY_DOMAIN_L1_CACHE, "CL_DEVICE_AFFINITY_DOMAIN_L1_CACHE" },
  { CL_DEVICE_AFFINITY_DOMAIN_NEXT_PARTITIONABLE, "CL_DEVICE_AFFINITY_DOMAIN_NEXT_PARTITIONABLE" },
};

static void pr_cl_device_affinity_domain(const char *name, void *value_ptr)
{
  cl_bitfield value = *(cl_bitfield *)value_ptr;

  printf("%s: ", name);

  if (value == 0)
  {
    printf("Not supported\n");
  }
  else
  {
    pr_cl_bitfield(value,
        lu_cl_device_affinity_domain,
        asize(lu_cl_device_affinity_domain, ocl_const_ulong_t));
  }
}

ocl_const_uint_t lu_cl_device_info[] = {
  { CL_DEVICE_TYPE, "CL_DEVICE_TYPE", pr_cl_device_type },
  { CL_DEVICE_VENDOR_ID, "CL_DEVICE_VENDOR_ID", pr_cl_uint },
  { CL_DEVICE_MAX_COMPUTE_UNITS, "CL_DEVICE_MAX_COMPUTE_UNITS", pr_cl_uint },
  { CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, "CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS", pr_cl_uint },
  { CL_DEVICE_MAX_WORK_GROUP_SIZE, "CL_DEVICE_MAX_WORK_GROUP_SIZE", pr_size_t },
  { CL_DEVICE_MAX_WORK_ITEM_SIZES, "CL_DEVICE_MAX_WORK_ITEM_SIZES", pr_size_t_arr },
  { CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, "CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR", pr_cl_uint },
  { CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, "CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT", pr_cl_uint },
  { CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, "CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT", pr_cl_uint },
  { CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, "CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG", pr_cl_uint },
  { CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, "CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT", pr_cl_uint },
  { CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, "CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE", pr_cl_uint },
  { CL_DEVICE_MAX_CLOCK_FREQUENCY, "CL_DEVICE_MAX_CLOCK_FREQUENCY", pr_cl_uint },
  { CL_DEVICE_ADDRESS_BITS, "CL_DEVICE_ADDRESS_BITS", pr_cl_uint },
  { CL_DEVICE_MAX_READ_IMAGE_ARGS, "CL_DEVICE_MAX_READ_IMAGE_ARGS", pr_cl_uint },
  { CL_DEVICE_MAX_WRITE_IMAGE_ARGS, "CL_DEVICE_MAX_WRITE_IMAGE_ARGS", pr_cl_uint },
  { CL_DEVICE_MAX_MEM_ALLOC_SIZE, "CL_DEVICE_MAX_MEM_ALLOC_SIZE", pr_cl_ulong },
  { CL_DEVICE_IMAGE2D_MAX_WIDTH, "CL_DEVICE_IMAGE2D_MAX_WIDTH", pr_size_t },
  { CL_DEVICE_IMAGE2D_MAX_HEIGHT, "CL_DEVICE_IMAGE2D_MAX_HEIGHT", pr_size_t },
  { CL_DEVICE_IMAGE3D_MAX_WIDTH, "CL_DEVICE_IMAGE3D_MAX_WIDTH", pr_size_t },
  { CL_DEVICE_IMAGE3D_MAX_HEIGHT, "CL_DEVICE_IMAGE3D_MAX_HEIGHT", pr_size_t },
  { CL_DEVICE_IMAGE3D_MAX_DEPTH, "CL_DEVICE_IMAGE3D_MAX_DEPTH", pr_size_t },
  { CL_DEVICE_IMAGE_SUPPORT, "CL_DEVICE_IMAGE_SUPPORT", pr_cl_bool },
  { CL_DEVICE_MAX_PARAMETER_SIZE, "CL_DEVICE_MAX_PARAMETER_SIZE", pr_size_t },
  { CL_DEVICE_MAX_SAMPLERS, "CL_DEVICE_MAX_SAMPLERS", pr_cl_uint },
  { CL_DEVICE_MEM_BASE_ADDR_ALIGN, "CL_DEVICE_MEM_BASE_ADDR_ALIGN", pr_cl_uint },
  { CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, "CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE", pr_cl_uint },
  { CL_DEVICE_SINGLE_FP_CONFIG, "CL_DEVICE_SINGLE_FP_CONFIG", pr_cl_device_fp_config },
  { CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, "CL_DEVICE_GLOBAL_MEM_CACHE_TYPE", pr_cl_device_mem_cache_type },
  { CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, "CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE", pr_cl_uint },
  { CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, "CL_DEVICE_GLOBAL_MEM_CACHE_SIZE", pr_cl_ulong },
  { CL_DEVICE_GLOBAL_MEM_SIZE, "CL_DEVICE_GLOBAL_MEM_SIZE", pr_cl_ulong },
  { CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, "CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE", pr_cl_ulong },
  { CL_DEVICE_MAX_CONSTANT_ARGS, "CL_DEVICE_MAX_CONSTANT_ARGS", pr_cl_uint },
  { CL_DEVICE_LOCAL_MEM_TYPE, "CL_DEVICE_LOCAL_MEM_TYPE", pr_cl_device_local_mem_type },
  { CL_DEVICE_LOCAL_MEM_SIZE, "CL_DEVICE_LOCAL_MEM_SIZE", pr_cl_ulong },
  { CL_DEVICE_ERROR_CORRECTION_SUPPORT, "CL_DEVICE_ERROR_CORRECTION_SUPPORT", pr_cl_bool },
  { CL_DEVICE_PROFILING_TIMER_RESOLUTION, "CL_DEVICE_PROFILING_TIMER_RESOLUTION", pr_size_t },
  { CL_DEVICE_ENDIAN_LITTLE, "CL_DEVICE_ENDIAN_LITTLE", pr_cl_bool },
  { CL_DEVICE_AVAILABLE, "CL_DEVICE_AVAILABLE", pr_cl_bool },
  { CL_DEVICE_COMPILER_AVAILABLE, "CL_DEVICE_COMPILER_AVAILABLE", pr_cl_bool },
  { CL_DEVICE_EXECUTION_CAPABILITIES, "CL_DEVICE_EXECUTION_CAPABILITIES", pr_cl_device_exec_capabilities },
  { CL_DEVICE_QUEUE_PROPERTIES, "CL_DEVICE_QUEUE_PROPERTIES", pr_cl_command_queue_properties},
  { CL_DEVICE_NAME, "CL_DEVICE_NAME", pr_char_arr },
  { CL_DEVICE_VENDOR, "CL_DEVICE_VENDOR", pr_char_arr },
  { CL_DRIVER_VERSION, "CL_DRIVER_VERSION", pr_char_arr },
  { CL_DEVICE_PROFILE, "CL_DEVICE_PROFILE", pr_char_arr },
  { CL_DEVICE_VERSION, "CL_DEVICE_VERSION", pr_char_arr },
  { CL_DEVICE_EXTENSIONS, "CL_DEVICE_EXTENSIONS", pr_char_arr },
  { CL_DEVICE_PLATFORM, "CL_DEVICE_PLATFORM", pr_cl_platform_id },
  { CL_DEVICE_DOUBLE_FP_CONFIG, "CL_DEVICE_DOUBLE_FP_CONFIG", pr_cl_device_fp_config },

  // Can only be used if cl_khr_fp16 extension is supported by the OpenCL
  // implementation.
  //{ CL_DEVICE_HALF_FP_CONFIG, "CL_DEVICE_HALF_FP_CONFIG", pr_cl_ulong }, //

  { CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF, "CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF", pr_cl_uint },
  { CL_DEVICE_HOST_UNIFIED_MEMORY, "CL_DEVICE_HOST_UNIFIED_MEMORY", pr_cl_bool },
  { CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR, "CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR", pr_cl_uint },
  { CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT, "CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT", pr_cl_uint },
  { CL_DEVICE_NATIVE_VECTOR_WIDTH_INT, "CL_DEVICE_NATIVE_VECTOR_WIDTH_INT", pr_cl_uint },
  { CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG, "CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG", pr_cl_uint },
  { CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT, "CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT", pr_cl_uint },
  { CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE, "CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE", pr_cl_uint },
  { CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF, "CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF", pr_cl_uint },
  { CL_DEVICE_OPENCL_C_VERSION, "CL_DEVICE_OPENCL_C_VERSION", pr_char_arr },
  { CL_DEVICE_LINKER_AVAILABLE, "CL_DEVICE_LINKER_AVAILABLE", pr_cl_bool },
  { CL_DEVICE_BUILT_IN_KERNELS, "CL_DEVICE_BUILT_IN_KERNELS", pr_char_arr },
  { CL_DEVICE_IMAGE_MAX_BUFFER_SIZE, "CL_DEVICE_IMAGE_MAX_BUFFER_SIZE", pr_size_t },
  { CL_DEVICE_IMAGE_MAX_ARRAY_SIZE, "CL_DEVICE_IMAGE_MAX_ARRAY_SIZE", pr_size_t },
  { CL_DEVICE_PARENT_DEVICE, "CL_DEVICE_PARENT_DEVICE", pr_cl_device_id },
  { CL_DEVICE_PARTITION_MAX_SUB_DEVICES, "CL_DEVICE_PARTITION_MAX_SUB_DEVICES", pr_cl_uint },
  { CL_DEVICE_PARTITION_PROPERTIES, "CL_DEVICE_PARTITION_PROPERTIES", pr_cl_device_partition_property },
  { CL_DEVICE_PARTITION_AFFINITY_DOMAIN, "CL_DEVICE_PARTITION_AFFINITY_DOMAIN", pr_cl_device_affinity_domain },
  { CL_DEVICE_PARTITION_TYPE, "CL_DEVICE_PARTITION_TYPE", pr_cl_device_partition_property },
  { CL_DEVICE_REFERENCE_COUNT, "CL_DEVICE_REFERENCE_COUNT", pr_cl_uint },
  { CL_DEVICE_PREFERRED_INTEROP_USER_SYNC, "CL_DEVICE_PREFERRED_INTEROP_USER_SYNC", pr_cl_bool },
  { CL_DEVICE_PRINTF_BUFFER_SIZE, "CL_DEVICE_PRINTF_BUFFER_SIZE", pr_size_t },

  // These two constants are missing in the specification, but present in the
  // header
  //{ CL_DEVICE_IMAGE_PITCH_ALIGNMENT, "CL_DEVICE_IMAGE_PITCH_ALIGNMENT", pr_cl_ulong },
  //{ CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT, "CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT", pr_cl_ulong },
};

static void err_common(cl_int err, const char *msg, va_list ap)
{
  const char *name;

  for (size_t i = 0; i < asize(lu_cl_error_codes, ocl_const_t); ++i)
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

void ocu_err_warn(cl_int err, const char *msg, ...)
{
  if (err != CL_SUCCESS)
  {
    va_list ap;
    va_start(ap, msg);
    err_common(err, msg, ap);
    va_end(ap);
  }
}

void ocu_err_exit(cl_int err, const char *msg, ...)
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

void ocu_info_dump()
{
  cl_int err;
  cl_platform_id *platform_ids = NULL;
  cl_device_id *device_ids = NULL;
  cl_uint platform_ids_size, device_ids_size;
  size_t i, j, k;
  void *info = NULL;
  size_t info_size;

  err = clGetPlatformIDs(0, NULL, &platform_ids_size);
  ocu_err_exit(err, "Failed clGetPlatformIDs");

  platform_ids = (cl_platform_id *)malloc(platform_ids_size *
      sizeof(cl_platform_id));

  err = clGetPlatformIDs(platform_ids_size, platform_ids, NULL);
  ocu_err_exit(err, "Failed clGetPlatformIDs");

  printf("Found %u OpenCL platforms.\n", platform_ids_size);

  for (i = 0; i < platform_ids_size; ++i)
  {
    printf("Platform[%zu]:\n", i);

    for (j = 0; j < asize(lu_cl_platform_info, ocl_const_uint_t); ++j)
    {
      ocl_const_uint_t pl_inf = lu_cl_platform_info[j];

      err = clGetPlatformInfo(platform_ids[i], pl_inf.value, 0, NULL,
          &info_size);
      ocu_err_exit(err, "Failed clGetPlatformInfo(%s)\n", pl_inf.name);

      info = realloc(info, info_size);

      err = clGetPlatformInfo(platform_ids[i], pl_inf.value, info_size, info,
          NULL);
      ocu_err_exit(err, "Failed clGetPlatformInfo(%s)\n", pl_inf.name);

      printf("%s : %s\n", pl_inf.name, info);
    }

    // Device info
    err = clGetDeviceIDs(platform_ids[i], CL_DEVICE_TYPE_ALL, 0, NULL,
        &device_ids_size);
    ocu_err_exit(err, "Failed clGetDeviceIDs(CL_DEVICE_TYPE_ALL)");

    device_ids = (cl_device_id *)realloc(device_ids, device_ids_size *
        sizeof(cl_device_id));

    err = clGetDeviceIDs(platform_ids[i], CL_DEVICE_TYPE_ALL, device_ids_size,
        device_ids, NULL);
    ocu_err_exit(err, "Failed clGetDeviceIDs(CL_DEVICE_TYPE_ALL)");

    printf("Found %u devices for Platform[%zu]\n", device_ids_size, i);

    for (j = 0; j < device_ids_size; ++j)
    {
      printf("Device[%zu]:\n", j);

      for (k = 0; k < asize(lu_cl_device_info, ocl_const_uint_t); ++k)
      {
        ocl_const_uint_t d_inf = lu_cl_device_info[k];

        err = clGetDeviceInfo(device_ids[j], d_inf.value, 0, NULL, &info_size);
        ocu_err_exit(err, "clGetDeviceInfo(%s)\n", d_inf.name);

        info = realloc(info, info_size);

        err = clGetDeviceInfo(device_ids[j], d_inf.value, info_size, info,
            NULL);
        ocu_err_exit(err, "clGetDeviceInfo(%s)\n", d_inf.name);

        d_inf.printer(d_inf.name, info, info_size);
      }
    }
  }

  free(info);
  free(device_ids);
  free(platform_ids);
}

static const ocl_const_uint_t lu_cl_channel_order[] = {
  { CL_R, "CL_R" },
  { CL_A, "CL_A" },
  { CL_RG, "CL_RG" },
  { CL_RA, "CL_RA" },
  { CL_RGB, "CL_RGB" },
  { CL_RGBA, "CL_RGBA" },
  { CL_BGRA, "CL_BGRA" },
  { CL_ARGB, "CL_ARGB" },
  { CL_INTENSITY, "CL_INTENSITY" },
  { CL_LUMINANCE, "CL_LUMINANCE" },
  { CL_Rx, "CL_Rx" },
  { CL_RGx, "CL_RGx" },
  { CL_RGBx, "CL_RGBx" },
  { CL_DEPTH, "CL_DEPTH" },
  { CL_DEPTH_STENCIL, "CL_DEPTH_STENCIL" },
};

static const ocl_const_uint_t lu_cl_channel_type[] = {
  { CL_SNORM_INT8, "CL_SNORM_INT8" },
  { CL_SNORM_INT16, "CL_SNORM_INT16" },
  { CL_UNORM_INT8, "CL_UNORM_INT8" },
  { CL_UNORM_INT16, "CL_UNORM_INT16" },
  { CL_UNORM_SHORT_565, "CL_UNORM_SHORT_565" },
  { CL_UNORM_SHORT_555, "CL_UNORM_SHORT_555" },
  { CL_UNORM_INT_101010, "CL_UNORM_INT_101010" },
  { CL_SIGNED_INT8, "CL_SIGNED_INT8" },
  { CL_SIGNED_INT16, "CL_SIGNED_INT16" },
  { CL_SIGNED_INT32, "CL_SIGNED_INT32" },
  { CL_UNSIGNED_INT8, "CL_UNSIGNED_INT8" },
  { CL_UNSIGNED_INT16, "CL_UNSIGNED_INT16" },
  { CL_UNSIGNED_INT32, "CL_UNSIGNED_INT32" },
  { CL_HALF_FLOAT, "CL_HALF_FLOAT" },
  { CL_FLOAT, "CL_FLOAT" },
  { CL_UNORM_INT24, "CL_UNORM_INT24" },
};

static void pr_cl_image_format(const char *name, void *value_ptr)
{
  cl_image_format value = *(cl_image_format *)value_ptr;

  printf("%s: {\n", name);
  printf("  image_channel_order: ");
  pr_cl_uint_lookup(value.image_channel_order, lu_cl_channel_order,
      asize(lu_cl_channel_order, ocl_const_uint_t));
  printf("  image_channel_data_type: ");
  pr_cl_uint_lookup(value.image_channel_data_type, lu_cl_channel_type,
      asize(lu_cl_channel_type, ocl_const_uint_t));
  printf("}\n");
}

static void pr_ptr(const char *name, void *value_ptr)
{
  printf("%s: %p\n", name, value_ptr);
}

static const ocl_const_uint_t lu_cl_image_info[] = {
  { CL_IMAGE_FORMAT, "CL_IMAGE_FORMAT", pr_cl_image_format },
  { CL_IMAGE_ELEMENT_SIZE, "CL_IMAGE_ELEMENT_SIZE", pr_size_t },
  { CL_IMAGE_ROW_PITCH, "CL_IMAGE_ROW_PITCH", pr_size_t },
  { CL_IMAGE_SLICE_PITCH, "CL_IMAGE_SLICE_PITCH", pr_size_t },
  { CL_IMAGE_WIDTH, "CL_IMAGE_WIDTH", pr_size_t },
  { CL_IMAGE_HEIGHT, "CL_IMAGE_HEIGHT", pr_size_t },
  { CL_IMAGE_DEPTH, "CL_IMAGE_DEPTH", pr_size_t },
  { CL_IMAGE_ARRAY_SIZE, "CL_IMAGE_ARRAY_SIZE", pr_size_t },
  { CL_IMAGE_BUFFER, "CL_IMAGE_BUFFER", pr_ptr },
  { CL_IMAGE_NUM_MIP_LEVELS, "CL_IMAGE_NUM_MIP_LEVELS", pr_cl_uint },
  { CL_IMAGE_NUM_SAMPLES, "CL_IMAGE_NUM_SAMPLES", pr_cl_uint },
  //{ CL_IMAGE_D3D10_SUBRESOURCE_KHR, "CL_IMAGE_D3D10_SUBRESOURCE_KHR" },
  //{ CL_IMAGE_D3D11_SUBRESOURCE_KHR, "CL_IMAGE_D3D11_SUBRESOURCE_KHR" },
  //{ CL_IMAGE_DX9_MEDIA_PLANE_KHR, "CL_IMAGE_DX9_MEDIA_PLANE_KHR" },
};

void ocu_image_info(cl_mem img)
{
  cl_int err;
  void *info = NULL;
  size_t info_size;

  printf("Image info for cl_mem %p :\n", img);

  for (size_t i = 0; i < asize(lu_cl_image_info, ocl_const_uint_t); ++i)
  {
    ocl_const_uint_t img_inf = lu_cl_image_info[i];

    err = clGetImageInfo(img, img_inf.value, 0, NULL, &info_size);
    ocu_err_exit(err, "Failed clGetImageInfo(%s)\n", img_inf.name);

    info = realloc(info, info_size);

    err = clGetImageInfo(img, img_inf.value, info_size, info, NULL);
    ocu_err_exit(err, "Failed clGetImageInfo(%s)\n", img_inf.name);

    img_inf.printer(img_inf.name, info, info_size);
  }

  free(info);
}
