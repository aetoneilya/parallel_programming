
__kernel void findSequenceGpu(__global int* C) {
  int i = get_global_id(0);

  int n = i;
  int len = 1;

  while (n > 1) {
    n = n % 2 == 0 ? n / 2 : 3 * n + 1;
    len++;
  }

  C[i] = len;
}

__kernel void ArrayMinimum(const uint elementCount, global float* values,
                           local float* localValues,
                           global uint* outGroupCount) {
  size_t globalIndex = get_global_id(0);
  size_t localIndex = get_local_id(0);
  size_t groupIndex = get_group_id(0);
  size_t localSize = get_local_size(0);

  if (globalIndex == 0) {
    *outGroupCount = get_num_groups(0);
  }

  localValues[localIndex] =
      (globalIndex < elementCount) ? values[globalIndex] : values[0];

  barrier(CLK_LOCAL_MEM_FENCE);

  for (int n = 2; n < 2 * localSize; n *= 2) {
    if (globalIndex < elementCount) {
      if ((localIndex % n == 0) && (localIndex + n / 2 < localSize)) {
        localValues[localIndex] =
            min(localValues[localIndex], localValues[localIndex + n / 2]);
      }
    }

    barrier(CLK_LOCAL_MEM_FENCE);
  }

  if (localIndex == 0) {
    values[groupIndex] = localValues[0];
  }
}