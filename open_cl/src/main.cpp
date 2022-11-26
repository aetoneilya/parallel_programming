// 33. Любое число n, большее единицы,
//     порождает последовательность вида :
//     ni + 1 = (ni % 2 == 0 ? ni / 2 : 3 * ni + 1),
//     последним элементом которой является 1
//     (например : 13->40->20->10->5->16->8->4->2->1).
//     Найти наибольшее, меньшее заданного N, число,
//     порождающее самую длинную такую последовательность.

#include <getopt.h>
#include <unistd.h>

#include "test.h"

void PrintHelp() {
  printf(
      "--compare:            Compares execution times of serial "
      "and parallel implementations\n"
      "--serial:             Runs serial implementations\n"
      "--parallel:           Runs parallel implementations\n"
      "--wgs <val>:          Set size of work group\n"
      "--N <val>:            Set size of N\n"
      "--outputSave:         Save results to file\n"
      "--help:               Show help\n");

  exit(1);
}

void ProcessArgs(int argc, char** argv) {
  const char* const short_opts = "cspw:n:oh";
  const option long_opts[] = {{"compare", no_argument, nullptr, 'c'},
                              {"serial", no_argument, nullptr, 's'},
                              {"parallel", no_argument, nullptr, 'p'},
                              {"wgs", required_argument, nullptr, 'w'},
                              {"N", required_argument, nullptr, 'n'},
                              {"inputFile", required_argument, nullptr, 'i'},
                              {"outputSave", no_argument, nullptr, 'o'},
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
      case 'w':
        flags.work_group_size = std::stoi(optarg);
        break;
      case 'n':
        flags.N = std::stoi(optarg);
        break;
      case 'o':
        flags.out_save = true;
        break;
      case 'h':
      case '?':
      default:
        PrintHelp();
        break;
    }
  }

  startTimeTest(flags);

  return;
}

int main(int argc, char* argv[]) {
  ProcessArgs(argc, argv);
  return 0;
}