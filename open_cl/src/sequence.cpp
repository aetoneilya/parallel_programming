#include "sequence.h"

#include <iostream>
int findLargestSequenceSerial(int N) {
  int best_len = 0;
  int best_n = 0;

  for (int n = 0; n < N; n++) {
    int tmp = sequenceLen(n);
    // std::cout << tmp << std::endl;
    if (tmp > best_len) {
      best_len = tmp;
      best_n = n;
    }
  }

  return best_n;
}

int findLargestSequenceParallel(int N) {
  int i;
  const int l_size = N;

  FILE *fp;
  char *source_str;
  size_t source_size;

  fp = fopen("src/sequence.cl", "r");
  if (!fp) {
    fprintf(stderr, "Failed to load kernel.\n");
    exit(1);
  }

  source_str = (char *)malloc(MAX_SOURCE_SIZE);
  source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
  fclose(fp);

  cl_platform_id platform_id = NULL;
  cl_device_id device_id = NULL;
  cl_uint ret_num_devices;
  cl_uint ret_num_platforms;
  cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
  ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id,
                       &ret_num_devices);

  cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

  cl_command_queue command_queue =
      clCreateCommandQueue(context, device_id, 0, &ret);

  cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
                                    l_size * sizeof(int), NULL, &ret);

  cl_program program =
      clCreateProgramWithSource(context, 1, (const char **)&source_str,
                                (const size_t *)&source_size, &ret);

  ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

  cl_kernel kernel = clCreateKernel(program, "findSequenceGpu", &ret);

  ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&c_mem_obj);

  size_t global_item_size = l_size;
  size_t local_item_size = 64;
  ret =
      clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size,
                             &local_item_size, 0, NULL, NULL);

  int *arr = (int *)malloc(sizeof(int) * l_size);
  ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0,
                            l_size * sizeof(int), arr, 0, NULL, NULL);

  //   for (i = 0; i < l_size; i++) printf("%d\n", arr[i]);
  int max = arr[0];
  int max_n = 1;
  for (int i = 0; i < N; i++)
    if (arr[i] > max) {
      max = arr[i];
      max_n = i;
    }

  ret = clFlush(command_queue);
  ret = clFinish(command_queue);
  ret = clReleaseKernel(kernel);
  ret = clReleaseProgram(program);
  ret = clReleaseMemObject(c_mem_obj);
  ret = clReleaseCommandQueue(command_queue);
  ret = clReleaseContext(context);
  free(arr);
  return max_n;
}

int sequenceLen(int n) {
  int len = 1;
  while (n > 1) {
    n = n % 2 == 0 ? n / 2 : 3 * n + 1;
    len++;
  }
  return len;
}