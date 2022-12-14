#ifndef CUDA_RADIX_SORT_RADIX_H_
#define CUDA_RADIX_SORT_RADIX_H_

#include <stdlib.h>
#include <time.h>

#include <iostream>

#include "scan.h"

#define THREADS_PER_BLOCK 128
#define UPPER_BIT 31
#define LOWER_BIT 0

void FillArrayRandom(uint* array, size_t N);

void PrintArray(uint* array, size_t N);

void SerialRadixSort(uint* array, size_t size);

__global__ void Count(uint* d_array, size_t arrary_size, int* d_count,
                      size_t count_size, int segment_size, int bit);

__global__ void Prefix(uint* d_array, size_t arrary_size, uint* d_buff,
                       int* d_count, size_t count_size, int segment_size,
                       int bit);

__global__ void MoveByPrefix(uint* d_array, size_t arrary_size, uint* d_buff);

void ParrallelRadixSort(uint* array, size_t size, int segment_size);

#endif  //  CUDA_RADIX_SORT_RADIX_H_