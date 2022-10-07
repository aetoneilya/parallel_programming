#include "scan.h"

#define MAX_BLOCK_SZ 128
#define NUM_BANKS 32
#define LOG_NUM_BANKS 5

#define CONFLICT_FREE_OFFSET(n) ((n) >> LOG_NUM_BANKS)

__global__ void gpu_add_block_sums(unsigned int* const d_out,
                                   const unsigned int* const d_in,
                                   unsigned int* const d_block_sums,
                                   const size_t numElems) {
  unsigned int d_block_sum_val = d_block_sums[blockIdx.x];

  unsigned int cpy_idx = 2 * blockIdx.x * blockDim.x + threadIdx.x;
  if (cpy_idx < numElems) {
    d_out[cpy_idx] = d_in[cpy_idx] + d_block_sum_val;
    if (cpy_idx + blockDim.x < numElems)
      d_out[cpy_idx + blockDim.x] =
          d_in[cpy_idx + blockDim.x] + d_block_sum_val;
  }
}

__global__ void gpu_prescan(unsigned int* const d_out,
                            const unsigned int* const d_in,
                            unsigned int* const d_block_sums,
                            const unsigned int len, const unsigned int shmem_sz,
                            const unsigned int max_elems_per_block) {
  extern __shared__ unsigned int s_out[];

  int thid = threadIdx.x;
  int ai = thid;
  int bi = thid + blockDim.x;

  s_out[thid] = 0;
  s_out[thid + blockDim.x] = 0;
  s_out[thid + blockDim.x + (blockDim.x >> LOG_NUM_BANKS)] = 0;

  __syncthreads();

  unsigned int cpy_idx = max_elems_per_block * blockIdx.x + threadIdx.x;
  if (cpy_idx < len) {
    s_out[ai + CONFLICT_FREE_OFFSET(ai)] = d_in[cpy_idx];
    if (cpy_idx + blockDim.x < len)
      s_out[bi + CONFLICT_FREE_OFFSET(bi)] = d_in[cpy_idx + blockDim.x];
  }

  int offset = 1;
  for (int d = max_elems_per_block >> 1; d > 0; d >>= 1) {
    __syncthreads();

    if (thid < d) {
      int ai = offset * ((thid << 1) + 1) - 1;
      int bi = offset * ((thid << 1) + 2) - 1;
      ai += CONFLICT_FREE_OFFSET(ai);
      bi += CONFLICT_FREE_OFFSET(bi);

      s_out[bi] += s_out[ai];
    }
    offset <<= 1;
  }

  if (thid == 0) {
    d_block_sums[blockIdx.x] =
        s_out[max_elems_per_block - 1 +
              CONFLICT_FREE_OFFSET(max_elems_per_block - 1)];
    s_out[max_elems_per_block - 1 +
          CONFLICT_FREE_OFFSET(max_elems_per_block - 1)] = 0;
  }

  for (int d = 1; d < max_elems_per_block; d <<= 1) {
    offset >>= 1;
    __syncthreads();

    if (thid < d) {
      int ai = offset * ((thid << 1) + 1) - 1;
      int bi = offset * ((thid << 1) + 2) - 1;
      ai += CONFLICT_FREE_OFFSET(ai);
      bi += CONFLICT_FREE_OFFSET(bi);

      unsigned int temp = s_out[ai];
      s_out[ai] = s_out[bi];
      s_out[bi] += temp;
    }
  }
  __syncthreads();

  if (cpy_idx < len) {
    d_out[cpy_idx] = s_out[ai + CONFLICT_FREE_OFFSET(ai)];
    if (cpy_idx + blockDim.x < len)
      d_out[cpy_idx + blockDim.x] = s_out[bi + CONFLICT_FREE_OFFSET(bi)];
  }
}

void sum_scan_blelloch(unsigned int* const d_out,
                       const unsigned int* const d_in, const size_t numElems) {
  checkCudaErrors(cudaMemset(d_out, 0, numElems * sizeof(unsigned int)));

  unsigned int block_sz = MAX_BLOCK_SZ / 2;
  unsigned int max_elems_per_block = 2 * block_sz;

  unsigned int grid_sz = numElems / max_elems_per_block;
  if (numElems % max_elems_per_block != 0) grid_sz += 1;

  unsigned int shmem_sz =
      max_elems_per_block + ((max_elems_per_block) >> LOG_NUM_BANKS);

  unsigned int* d_block_sums;
  checkCudaErrors(cudaMalloc(&d_block_sums, sizeof(unsigned int) * grid_sz));
  checkCudaErrors(cudaMemset(d_block_sums, 0, sizeof(unsigned int) * grid_sz));

  gpu_prescan<<<grid_sz, block_sz, sizeof(unsigned int) * shmem_sz>>>(
      d_out, d_in, d_block_sums, numElems, shmem_sz, max_elems_per_block);

  if (grid_sz <= max_elems_per_block) {
    unsigned int* d_dummy_blocks_sums;
    checkCudaErrors(cudaMalloc(&d_dummy_blocks_sums, sizeof(unsigned int)));
    checkCudaErrors(cudaMemset(d_dummy_blocks_sums, 0, sizeof(unsigned int)));
    gpu_prescan<<<1, block_sz, sizeof(unsigned int) * shmem_sz>>>(
        d_block_sums, d_block_sums, d_dummy_blocks_sums, grid_sz, shmem_sz,
        max_elems_per_block);
    checkCudaErrors(cudaFree(d_dummy_blocks_sums));
  } else {
    unsigned int* d_in_block_sums;
    checkCudaErrors(
        cudaMalloc(&d_in_block_sums, sizeof(unsigned int) * grid_sz));
    checkCudaErrors(cudaMemcpy(d_in_block_sums, d_block_sums,
                               sizeof(unsigned int) * grid_sz,
                               cudaMemcpyDeviceToDevice));
    sum_scan_blelloch(d_block_sums, d_in_block_sums, grid_sz);
    checkCudaErrors(cudaFree(d_in_block_sums));
  }
  gpu_add_block_sums<<<grid_sz, block_sz>>>(d_out, d_out, d_block_sums,
                                            numElems);

  checkCudaErrors(cudaFree(d_block_sums));
}
