/******************************************************************************
 * Copyright © 2017 René Hansen
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

/**
 * Changelog
 *
 * 2018-03-25 - 1.0.0
 *
 * - Introduces initial versioning.
 * - Adds Android specific printf and vprintf aliasing.
 *
 */

/**
 *
 * opencl_util
 *
 * Various utility functions to ease working with OpenCL.
 *
 */

#ifndef OPENCL_INFO_H
#define OPENCL_INFO_H

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

  /**
   * ocu_err_warn
   *
   * Checks err against CL_SUCCESS and in case of failure, prints error code as
   * text, followed by msg. Treat msg as a variadic string format in the style
   * of printf.
   */
  void ocu_err_warn(cl_int err, const char *msg, ...);

  /**
   * ocu_err_exit
   *
   * Like err warn, but exits with EXIT_FAILURE
   */
  void ocu_err_exit(cl_int err, const char *msg, ...);

  /**
   * ocu_info_dump
   *
   * Iterates each platform, as well as all devices for each platform, on the
   * system and prints out all obtainable information from clGetPlatformInfo and
   * clGetDeviceInfo.
   */
  void ocu_info_dump();

  /**
   * ocu_platform_info
   *
   * Prints available information from clGetPlatformInfo for a cl_platform_id
   * instance.
   */
  void ocu_platform_info(cl_platform_id p_id);

  /**
   * ocu_device_info
   *
   * Prints available information from clGetDeviceInfo for a cl_device_id
   * instance.
   */
  void ocu_device_info(cl_device_id d_id);

  /**
   * ocu_image_info
   *
   * Prints available information from clGetImageInfo for a cl_mem instance.
   */
  void ocu_image_info(cl_mem img);

  /**
   * ocu_queue_info
   *
   * Prints available information from clGetCommandQueueInfo for a
   * cl_command_queue  instance.
   */
  void ocu_queue_info(cl_command_queue q);

  /**
   * ocu_context_pfn_notify
   *
   * Basic callback function to use as pfn_notify for clCreateContext. Prints
   * errinfo and disregards private_info and user_data.
   */
  void ocu_context_pfn_notify(const char *errinfo, const void *private_info,
      size_t cb, void *user_data);

#ifdef __cplusplus
}
#endif

#endif
