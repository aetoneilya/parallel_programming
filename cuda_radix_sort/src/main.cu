
/*
8:
Поразрядная сортировка
*/

#include <getopt.h>
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
  const char* const short_opts = "cspb:a:i:odg:h";
  const option long_opts[] = {{"compare", no_argument, nullptr, 'c'},
                              {"serial", no_argument, nullptr, 's'},
                              {"parallel", no_argument, nullptr, 'p'},
                              {"block", required_argument, nullptr, 'b'},
                              {"arraySize", required_argument, nullptr, 'a'},
                              {"inputFile", required_argument, nullptr, 'i'},
                              {"outputSave", no_argument, nullptr, 'o'},
                              {"debug", no_argument, nullptr, 'd'},
                              {"genInput", required_argument, nullptr, 'g'},
                              {"help", no_argument, nullptr, 'h'},
                              {nullptr, no_argument, nullptr, 0}};

  TestFlags flags;

  while (true) {
    const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

    if (opt == -1) break;

    switch (opt) {
      case 'c':
        flags.test_type = TestType::Compare;
        break;
      case 's':
        flags.test_type = TestType::Serial;
        break;
      case 'p':
        flags.test_type = TestType::Parallel;
        break;
      case 'b':
        flags.block_size = std::stoi(optarg);
        break;
      case 'a':
        flags.array_size = atoll(optarg);
        break;
      case 'i':
        flags.input_file = std::string(optarg);
        break;
      case 'o':
        flags.out_save = true;
        break;
      case 'd':
        flags.debug = true;
        break;
      case 'g':
        flags.gen_file = std::string(optarg);
        break;
      case 'h':
      case '?':
      default:
        PrintHelp();
        break;
    }
  }

  if (flags.gen_file != "") {
    GenerateInput(flags.gen_file, flags.array_size);
  }

  uint* array = new uint[flags.array_size];

  FillArray(array, flags.array_size, flags.input_file);
  RadixTest(array, flags);

  delete[] array;
  return;
}

int main(int argc, char* argv[]) {
  ProcessArgs(argc, argv);
  return 0;
}