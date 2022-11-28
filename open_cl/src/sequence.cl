
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