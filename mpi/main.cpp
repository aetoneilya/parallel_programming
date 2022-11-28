// 57. Найти наименьшее простое число, большее заданного N,
//     которое является суммой заданного количества k
//     последовательно возрастающих простых чисел.

#include "common/common.h"
#include "common/timer.h"

void StartTimeTestSerial() {
  int n = 0;
  int k = 0;
  std::cout << "Введите N: ";
  std::cin >> n;
  std::cout << "Введите k: ";
  std::cin >> k;

  Timer t;
  PrimeLine pl(0, k);

  t.Start();
  while (pl.sum_ != n) {
    n = NextPrime(n);
    pl.Init(0, k);

    while (pl.sum_ < n) pl.Next();
  }
  t.End();

  std::cout << n << " = " << pl.first_;
  int tmp = pl.first_;
  for (int i = 1; i < k; i++) {
    tmp = NextPrime(tmp);
    std::cout << " + " << tmp;
  }
  std::cout << std::endl << "Затраченное время: " << t.GetNanosec() << " нс\n";
}

int main() {
  StartTimeTestSerial();
  return 0;
}