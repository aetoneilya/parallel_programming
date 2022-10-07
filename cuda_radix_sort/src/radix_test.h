#ifndef CUDA_RADIX_SORT_RADIX_TEST_H_
#define CUDA_RADIX_SORT_RADIX_TEST_H_

#include <cstdlib>
#include <fstream>
#include <sstream>

#include "radix.h"
#include "timer.h"

enum TestType { Compare = 0, Serial, Parallel };

void RadixTest(uint* array, size_t size, TestType type, int segment_size) {
  Timer timer;

  uint* array_serial = new uint[size];
  std::copy(array, array + size, array_serial);

  if (type == TestType::Compare || type == TestType::Serial) {
    // PrintArray(array_serial, size);
    timer.Start();
    SerialRadixSort(array_serial, size);
    timer.End();
    std::cout << "Serial   : " << timer.GetSecond() << " " << timer.GetNanosec()
              << std::endl;
    // PrintArray(array_serial, size);
  }

  if (type == TestType::Compare || type == TestType::Parallel) {
    timer.Start();
    // PrintArray(array, size);
    ParrallelRadixSort(array, size, segment_size);
    timer.End();
    std::cout << "Parralel : " << timer.GetSecond() << " " << timer.GetNanosec()
              << std::endl;
    // PrintArray(array, size);
  }

  if (type == TestType::Compare) {
    bool match = true;
    for (size_t i = 0; i < size; i++) {
      if (array[i] != array_serial[i]) {
        match = false;
        break;
      }
    }
    if (match) std::cout << "Success!\n";
    else std::cout << "Arrays dont match!\n";
  }

  delete array_serial;
}

void FillArrayRandom(uint* array, size_t N) {
  srand((uint)time(NULL));
  for (size_t i = 0; i < N; i++) {
    array[i] = rand() % 20;
  }
}

void FillArrayFromFile(uint* array, size_t size, std::string input_file) {
  std::ifstream file(input_file);
  int count = 0;
  if (file.is_open()) {
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line) && count < size) {
      std::istringstream iss(line);
      iss >> array[count++];
    }
    file.close();
  } else {
    throw std::invalid_argument("Can't open file");
  }
}

void FillArray(uint* array, size_t size, std::string input_file) {
  if (input_file != "") {
    FillArrayFromFile(array, size, input_file);
  } else {
    FillArrayRandom(array, size);
  }
}

void PrintArray(uint* array, size_t N) {
  for (size_t i = 0; i < N; i++) {
    std::cout << array[i] << " ";
  }
  std::cout << std::endl;
}

#endif  // CUDA_RADIX_SORT_RADIX_TEST_H_