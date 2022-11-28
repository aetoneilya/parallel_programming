// 57. Найти наименьшее простое число, большее заданного N,
//     которое является суммой заданного количества k
//     последовательно возрастающих простых чисел.

#include "common/common.h"
#include "common/timer.h"
#include "mpi.h"


int main(int argc, char **argv) {
  int size, rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  size_t n = 0;
  size_t k = 0;
  if (rank == 0) {
    std::cout << "Введите N: ";
    std::cin >> n;
    std::cout << "Введите k: ";
    std::cin >> k;
  }

  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);

  PrimeLine pl(0, k);

  Timer t;

  if (rank == 0) t.Start();

  n = NextPrime(n, rank + 1);
  size_t result = 2147483640;
  size_t g_min_result = 2147483640;

  bool flag = true;
  while (flag) {
    pl.Init(0, k);
    while (pl.sum_ < n) pl.Next();

    if (pl.sum_ == n) {
      result = n;
      flag = false;
    }

    MPI_Allreduce(&result, &g_min_result, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);

    n = NextPrime(n, size);
    if (g_min_result <= n && g_min_result <= result) {
      result = g_min_result;
      flag = false;
    }
  }

  if (rank == 0) {
    result = g_min_result;
    t.End();

    pl.Init(0, k);
    while (pl.sum_ < result) pl.Next();

    std::cout << result << " = " << pl.first_;
    size_t tmp = pl.first_;
    for (size_t i = 1; i < k; i++) {
      tmp = NextPrime(tmp);
      std::cout << " + " << tmp;
    }
    std::cout << std::endl
              << "Затраченное время: " << t.GetNanosec() << " нс\n";
  }

  MPI_Finalize();
  return 0;
}