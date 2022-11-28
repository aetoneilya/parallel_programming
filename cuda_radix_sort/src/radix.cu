#include "radix.h"

void SerialRadixSort(uint* array, size_t size) {
  uint* buff = new uint[size];

  for (uint bit = LOWER_BIT; bit <= UPPER_BIT; bit++) {
    size_t count_zero = 0;

    for (size_t i = 0; i < size; i++) {
      if (!((array[i] >> bit) & 1)) count_zero++;
    }

    size_t zero_ind = 0;
    size_t one_ind = count_zero;

    for (size_t i = 0; i < size; i++) {
      ((array[i] >> bit) & 1) ? buff[one_ind++] = array[i]
                              : buff[zero_ind++] = array[i];
    }

    std::swap(array, buff);
  }

  delete[] buff;
}

__global__ void gpu_radix_sort_local(unsigned int* d_out_sorted,
                                     unsigned int* d_prefix_sums,
                                     unsigned int* d_block_sums,
                                     unsigned int input_shift_width,
                                     unsigned int* d_in, unsigned int d_in_len,
                                     unsigned int max_elems_per_block) {
  extern __shared__ unsigned int shmem[];
  unsigned int* s_data = shmem;
  unsigned int s_mask_out_len = max_elems_per_block + 1;
  unsigned int* s_mask_out = &s_data[max_elems_per_block];
  unsigned int* s_merged_scan_mask_out = &s_mask_out[s_mask_out_len];
  unsigned int* s_mask_out_sums = &s_merged_scan_mask_out[max_elems_per_block];
  unsigned int* s_scan_mask_out_sums = &s_mask_out_sums[4];

  unsigned int thid = threadIdx.x;

  unsigned int cpy_idx = max_elems_per_block * blockIdx.x + thid;
  if (cpy_idx < d_in_len)
    s_data[thid] = d_in[cpy_idx];
  else
    s_data[thid] = 0;

  __syncthreads();

  unsigned int t_data = s_data[thid];
  unsigned int t_2bit_extract = (t_data >> input_shift_width) & 3;

  for (unsigned int i = 0; i < 4; ++i) {
    s_mask_out[thid] = 0;
    if (thid == 0) s_mask_out[s_mask_out_len - 1] = 0;

    __syncthreads();

    bool val_equals_i = false;
    if (cpy_idx < d_in_len) {
      val_equals_i = t_2bit_extract == i;
      s_mask_out[thid] = val_equals_i;
    }
    __syncthreads();

    int partner = 0;
    unsigned int sum = 0;
    unsigned int max_steps = (unsigned int)log2f(max_elems_per_block);
    for (unsigned int d = 0; d < max_steps; d++) {
      partner = thid - (1 << d);
      if (partner >= 0) {
        sum = s_mask_out[thid] + s_mask_out[partner];
      } else {
        sum = s_mask_out[thid];
      }
      __syncthreads();
      s_mask_out[thid] = sum;
      __syncthreads();
    }

    unsigned int cpy_val = 0;
    cpy_val = s_mask_out[thid];
    __syncthreads();
    s_mask_out[thid + 1] = cpy_val;
    __syncthreads();

    if (thid == 0) {
      s_mask_out[0] = 0;
      unsigned int total_sum = s_mask_out[s_mask_out_len - 1];
      s_mask_out_sums[i] = total_sum;
      d_block_sums[i * gridDim.x + blockIdx.x] = total_sum;
    }
    __syncthreads();

    if (val_equals_i && (cpy_idx < d_in_len)) {
      s_merged_scan_mask_out[thid] = s_mask_out[thid];
    }

    __syncthreads();
  }

  if (thid == 0) {
    unsigned int run_sum = 0;
    for (unsigned int i = 0; i < 4; ++i) {
      s_scan_mask_out_sums[i] = run_sum;
      run_sum += s_mask_out_sums[i];
    }
  }

  __syncthreads();

  if (cpy_idx < d_in_len) {
    unsigned int t_prefix_sum = s_merged_scan_mask_out[thid];
    unsigned int new_pos = t_prefix_sum + s_scan_mask_out_sums[t_2bit_extract];

    __syncthreads();

    s_data[new_pos] = t_data;
    s_merged_scan_mask_out[new_pos] = t_prefix_sum;

    __syncthreads();

    d_prefix_sums[cpy_idx] = s_merged_scan_mask_out[thid];
    d_out_sorted[cpy_idx] = s_data[thid];
  }
}

__global__ void gpu_glbl_shuffle(unsigned int* d_out, unsigned int* d_in,
                                 unsigned int* d_scan_block_sums,
                                 unsigned int* d_prefix_sums,
                                 unsigned int input_shift_width,
                                 unsigned int d_in_len,
                                 unsigned int max_elems_per_block) {
  unsigned int thid = threadIdx.x;
  unsigned int cpy_idx = max_elems_per_block * blockIdx.x + thid;

  if (cpy_idx < d_in_len) {
    unsigned int t_data = d_in[cpy_idx];
    unsigned int t_2bit_extract = (t_data >> input_shift_width) & 3;
    unsigned int t_prefix_sum = d_prefix_sums[cpy_idx];
    unsigned int data_glbl_pos =
        d_scan_block_sums[t_2bit_extract * gridDim.x + blockIdx.x] +
        t_prefix_sum;
    __syncthreads();
    d_out[data_glbl_pos] = t_data;
  }
}

void ParrallelRadixSort(uint* array, size_t size, int segment_size) {
  unsigned int block_sz = segment_size;
  unsigned int max_elems_per_block = block_sz;
  unsigned int grid_sz = size / max_elems_per_block;
  if (size % max_elems_per_block != 0) grid_sz += 1;

  unsigned int* d_out;
  unsigned int* d_in;
  cudaMalloc(&d_in, sizeof(uint) * size);
  cudaMalloc(&d_out, sizeof(uint) * size);
  cudaMemcpy(d_in, (void*)array, sizeof(uint) * size, cudaMemcpyHostToDevice);

  unsigned int* d_prefix_sums;
  unsigned int d_prefix_sums_len = size;
  checkCudaErrors(
      cudaMalloc(&d_prefix_sums, sizeof(unsigned int) * d_prefix_sums_len));
  checkCudaErrors(
      cudaMemset(d_prefix_sums, 0, sizeof(unsigned int) * d_prefix_sums_len));

  unsigned int* d_block_sums;
  unsigned int d_block_sums_len = 4 * grid_sz;
  checkCudaErrors(
      cudaMalloc(&d_block_sums, sizeof(unsigned int) * d_block_sums_len));
  checkCudaErrors(
      cudaMemset(d_block_sums, 0, sizeof(unsigned int) * d_block_sums_len));

  unsigned int* d_scan_block_sums;
  checkCudaErrors(
      cudaMalloc(&d_scan_block_sums, sizeof(unsigned int) * d_block_sums_len));
  checkCudaErrors(cudaMemset(d_scan_block_sums, 0,
                             sizeof(unsigned int) * d_block_sums_len));

  unsigned int s_data_len = max_elems_per_block;
  unsigned int s_mask_out_len = max_elems_per_block + 1;
  unsigned int s_merged_scan_mask_out_len = max_elems_per_block;
  unsigned int s_mask_out_sums_len = 4;
  unsigned int s_scan_mask_out_sums_len = 4;
  unsigned int shmem_sz =
      (s_data_len + s_mask_out_len + s_merged_scan_mask_out_len +
       s_mask_out_sums_len + s_scan_mask_out_sums_len) *
      sizeof(unsigned int);

  for (unsigned int shift_width = 0; shift_width <= 30; shift_width += 2) {
    gpu_radix_sort_local<<<grid_sz, block_sz, shmem_sz>>>(
        d_out, d_prefix_sums, d_block_sums, shift_width, d_in, size,
        max_elems_per_block);

    sum_scan_blelloch(d_scan_block_sums, d_block_sums, d_block_sums_len);

    gpu_glbl_shuffle<<<grid_sz, block_sz>>>(d_in, d_out, d_scan_block_sums,
                                            d_prefix_sums, shift_width, size,
                                            max_elems_per_block);
  }

  cudaMemcpy(array, (void*)d_in, sizeof(uint) * size, cudaMemcpyDeviceToHost);

  checkCudaErrors(cudaFree(d_scan_block_sums));
  checkCudaErrors(cudaFree(d_block_sums));
  checkCudaErrors(cudaFree(d_prefix_sums));
}
