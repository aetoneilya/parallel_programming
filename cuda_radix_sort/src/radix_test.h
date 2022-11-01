#ifndef CUDA_RADIX_SORT_RADIX_TEST_H_
#define CUDA_RADIX_SORT_RADIX_TEST_H_

#include <cstdlib>
#include <fstream>
#include <sstream>

#include "radix.h"
#include "timer.h"

void SaveArray(uint* array, size_t size, std::string file_name);

enum TestType { Compare = 0, Serial, Parallel };

struct TestFlags {
  TestType test_type = TestType::Compare;
  size_t block_size = 128;
  size_t array_size = 100000000;
  std::string input_file;
  std::string gen_file = "";
  bool out_save = false;
  bool debug = false;
};

void RadixTest(uint* array, TestFlags flags) {
  Timer timer;
  TestType type = flags.test_type;
  size_t size = flags.array_size;
  std::cout << "Array size: " << size << std::endl;
  uint* array_serial = new uint[size];
  std::copy(array, array + size, array_serial);

  if (flags.debug) {
    std::cout << "Debug ON:" << std::endl
              << "Input data saved in parralel_input_array.txt and "
                 "serial_input_array.txt"
              << std::endl;
    SaveArray(array, size, "parralel_input_array.txt");
    SaveArray(array_serial, size, "serial_input_array.txt");
  }

  long int serial_time, parallel_time;
  if (type == TestType::Compare || type == TestType::Serial) {
    timer.Start();
    SerialRadixSort(array_serial, size);
    timer.End();
    serial_time = timer.GetNanosec();
    std::cout << "Serial   : " << timer.GetSecond() << " " << timer.GetNanosec()
              << std::endl;
  }

  if (type == TestType::Compare || type == TestType::Parallel) {
    timer.Start();
    ParrallelRadixSort(array, size, flags.block_size);
    timer.End();
    parallel_time = timer.GetNanosec();
    std::cout << "Parralel : " << timer.GetSecond() << " " << timer.GetNanosec()
              << std::endl;
  }

  if (type == TestType::Compare) {
    long difference = serial_time - parallel_time;
    double boost = 100 * (double)std::max(parallel_time, serial_time) /
                       (double)std::min(parallel_time, serial_time) -
                   100;
    std::cout << (difference > 0 ? "Parallel" : "Serial")
              << " version is faster by "
              << (difference > 0 ? difference : -difference) << "ns\n"
              << "That's " << boost << "% boost!\n";

    bool match = true;
    for (size_t i = 0; i < size; i++) {
      if (array[i] != array_serial[i]) {
        match = false;
        break;
      }
    }
    if (match)
      std::cout << "Success!\n";
    else
      std::cout << "Arrays dont match!\n";
  }

  if (flags.debug) {
    std::cout << "Output data saved in parralel_output_array.txt and "
                 "serial_output_array.txt"
              << std::endl;
    SaveArray(array, size, "parralel_output_array.txt");
    SaveArray(array_serial, size, "serial_output_array.txt");
  }

  delete[] array_serial;
}

void FillArrayRandom(uint* array, size_t N) {
  srand((uint)time(NULL));
  for (size_t i = 0; i < N; i++) {
    array[i] = rand();
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
    std::cerr << "Can't open file" << std::endl;
    exit(1);
    // throw std::invalid_argument("Can't open file");
  }
}

void FillArray(uint* array, size_t size, std::string input_file) {
  if (input_file != "") {
    FillArrayFromFile(array, size, input_file);
  } else {
    FillArrayRandom(array, size);
  }
}

void GenerateInput(std::string file_name, size_t size) {
  uint* array = new uint[size];
  FillArrayRandom(array, size);
  SaveArray(array, size, file_name);
  delete[] array;
}

void PrintArray(uint* array, size_t N) {
  for (size_t i = 0; i < N; i++) {
    std::cout << array[i] << " ";
  }
  std::cout << std::endl;
}

void SaveArray(uint* array, size_t size, std::string file_name) {
  std::ofstream out(file_name);

  for (size_t i = 0; i < size; i++) {
    out << array[i] << ' ';
  }

  out.close();
}

#endif  // CUDA_RADIX_SORT_RADIX_TEST_H_