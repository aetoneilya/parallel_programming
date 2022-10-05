
/*
8:
Поразрядная сортировка
*/

#include <getopt.h>
// #include <string.h>
#include <unistd.h>

#include "radix_test.h"

void PrintHelp() {
  printf(
      "--compare:            Compares execution times of serial "
      "and parallel implementations\n"
      "--serial:             Runs serial implementations\n"
      "--parallel:           Runs parallel implementations\n"
      "--block <val>:        Set size of parrallel block\n"
      "--arraySize <val>:    Set size of array\n"
      "--inputFile <fname>:  File with input data\n"
      "--outputFile <fname>: File to save the result \n"
      "--genInput <fname>:   Generate random input file\n"
      "--help:               Show help\n");

  exit(1);
}

void ProcessArgs(int argc, char** argv) {
  const char* const short_opts = "cspb:a:i:o:g:h";
  const option long_opts[] = {{"compare", no_argument, nullptr, 'c'},
                              {"serial", no_argument, nullptr, 's'},
                              {"parallel", no_argument, nullptr, 'p'},
                              {"block", required_argument, nullptr, 'b'},
                              {"arraySize", required_argument, nullptr, 'a'},
                              {"inputFile", required_argument, nullptr, 'i'},
                              {"outputFile", required_argument, nullptr, 'o'},
                              {"genInput", required_argument, nullptr, 'g'},
                              {"help", no_argument, nullptr, 'h'},
                              {nullptr, no_argument, nullptr, 0}};

  TestType test_type = TestType::Compare;
  size_t block_size = 128;
  size_t array_size = 100000000;
  std::string input_file;
  std::string output_file;

  while (true) {
    const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

    if (-1 == opt) break;

    switch (opt) {
      case 'c':
        test_type = TestType::Compare;
        break;
      case 's':
        test_type = TestType::Serial;
        break;
      case 'p':
        test_type = TestType::Parallel;
        break;
      case 'b':
        block_size = std::stoi(optarg);
        break;
      case 'a':
        array_size = atoll(optarg);
        break;
      case 'i':
        input_file = std::string(optarg);
        break;
      case 'o':
        output_file = std::string(optarg);
        break;
      case 'g':
        // Generate data
        break;
      case 'h':
      case '?':
      default:
        PrintHelp();
        break;
    }
  }

  uint64_t* array = new uint64_t[array_size];

  FillArray(array, array_size, input_file);
  RadixTest(array, array_size, test_type, block_size);

  delete[] array;
  return;
}

int main(int argc, char* argv[]) {
  ProcessArgs(argc, argv);
  return 0;
}