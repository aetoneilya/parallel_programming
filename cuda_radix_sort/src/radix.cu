#include "radix.h"

void SerialRadixSort(uint64_t* array, size_t size) {
  uint64_t* buff = new uint64_t[size];

  for (int bit = LOWER_BIT; bit <= UPPER_BIT; bit++) {
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

__global__ void Count(uint64_t* d_array, size_t arrary_size, int* d_count,
                      size_t count_size, int segment_size, int bit) {
  int index = threadIdx.x + blockDim.x * blockIdx.x;

  int count_zero = 0;
  int count_one = 0;

  for (int i = 0; i < segment_size; i++) {
    //!
    if ((index * segment_size + i) >= arrary_size) break;

    if ((d_array[index * segment_size + i] >> bit) & 1) {
      count_one++;
    } else {
      count_zero++;
    }
  }

  d_count[index * 2] = count_zero;
  d_count[index * 2 + 1] = count_one;
}

__global__ void Prefix(uint64_t* d_array, size_t arrary_size, uint64_t* d_buff,
                       int* d_count, size_t count_size, int segment_size,
                       int bit) {
  int index = threadIdx.x + blockDim.x * blockIdx.x;

  int count_zero = 0;
  int count_one = 0;

  for (int i = 0; i < count_size/2; i++) {
    count_one += d_count[i*2];
  }

  for (int i = 0; i < index; i++) {
    count_zero += d_count[i * 2];
    count_one += d_count[i * 2 + 1];
  }

//   if (index == 0) {
//     for (int i = 0; i < count_size; i++) {
//       printf("dc %d\n", d_count[i]);
//     }
//   }

//   printf("index %d | count one %d | count zero %d\n", index, count_one,
//          count_zero);

  for (int i = 0; i < segment_size; i++) {
    //!
    if ((index * segment_size + i) >= arrary_size) break;;

    if ((d_array[index * segment_size + i] >> bit) & 1) {
      d_buff[index * segment_size + i] = count_one++;
    } else {
      d_buff[index * segment_size + i] = count_zero++;
    }
  }
}

__global__ void MoveByPrefix(uint64_t* d_array, size_t arrary_size,
                             uint64_t* d_buff) {
  int index = threadIdx.x + blockDim.x * blockIdx.x;

  if (index < arrary_size) {
    d_buff[d_buff[index]] = d_array[index];
  }
}

void ParrallelRadixSort(uint64_t* array, size_t size, int segment_size) {
  uint64_t* d_array;
  uint64_t* d_buff;
  cudaMalloc(&d_array, sizeof(uint64_t) * size);
  cudaMalloc(&d_buff, sizeof(uint64_t) * size);
  cudaMemcpy(d_array, (void*)array, sizeof(uint64_t) * size,
             cudaMemcpyHostToDevice);

  segment_size = THREADS_PER_BLOCK;
  int blocks = ceil(size / (double)THREADS_PER_BLOCK);

  int* d_count;
  size_t count_size = blocks * 2;
  cudaMalloc(&d_count, sizeof(int) * count_size);

  printf("blocks %d\tsegment size: %i\tarray size: %lu\tcount size: %lu\n",
         blocks, segment_size, size, count_size);

  for (int bit = LOWER_BIT; bit <= UPPER_BIT; bit++) {
    Count<<<blocks, 1>>>(d_array, size, d_count, count_size, segment_size, bit);
    cudaDeviceSynchronize();
    Prefix<<<blocks, 1>>>(d_array, size, d_buff, d_count, count_size,
                          segment_size, bit);
    cudaDeviceSynchronize();
    MoveByPrefix<<<blocks, THREADS_PER_BLOCK>>>(d_array, size, d_buff);
    cudaDeviceSynchronize();

    std::swap(d_array, d_buff);
    // break;
  }

  cudaMemcpy(array, (void*)d_array, sizeof(uint64_t) * size,
             cudaMemcpyDeviceToHost);

  //   printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
  //   PrintArray(array, size);

  cudaFree(d_array);
  cudaFree(d_buff);
  cudaFree(d_count);
}
