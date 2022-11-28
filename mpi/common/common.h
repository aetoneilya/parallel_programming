#include <iostream>

bool IsPrime(int number) {
  if (number == 2 || number == 3) return true;
  if (number % 2 == 0 || number % 3 == 0) return false;

  int divisor = 6;
  while (divisor * divisor - 2 * divisor + 1 <= number) {
    if (number % (divisor - 1) == 0) return false;
    if (number % (divisor + 1) == 0) return false;
    divisor += 6;
  }

  return true;
}

int NextPrime(int a) {
  while (!IsPrime(++a)) {
  }
  return a;
}

int NextPrime(int a, int n) {
  for (size_t i = 0; i < n; i++)
    while (!IsPrime(++a)) {
    }

  return a;
}

struct PrimeLine {
  size_t sum_ = 0;
  size_t first_ = 0;
  size_t last_ = 0;

  PrimeLine(size_t n, size_t k) { Init(n, k); }

  void Init(size_t n, size_t k) {
    if (!IsPrime(n)) n = NextPrime(n);
    first_ = n;
    sum_ = first_;

    for (int i = 1; i < k; i++) {
      n = NextPrime(n);
      sum_ += n;
    }
    last_ = n;
  }

  void Next() {
    sum_ -= first_;
    first_ = NextPrime(first_);
    last_ = NextPrime(last_);
    sum_ += last_;
  }
};