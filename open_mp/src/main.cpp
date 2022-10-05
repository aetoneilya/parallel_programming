/*
21:
Решение задачи движения N тел в трехмерном пространстве с учетом сил
гравитационного притяжения между ними; при упругих соударениях количество тел не
меняется.
Массы, радиусы, начальные координаты и векторы скоростей всех тел
считать заданными. Вычислить координаты всех тел через время T.
*/

#include <getopt.h>

#include <iostream>

#include "n_body_tester.h"

void PrintHelp() {
  std::cout << "--compare:               Compares execution times of serial "
               "and parallel implementations\n"
               "--serial:                Runs serial implementations\n"
               "--parallel:              Runs parallel implementations\n"
               "--threads <val>:         Set number of threads\n"
               "--bodyCount <val>:       Set number of bodies\n"
               "--momentTime <val>:      Set number moment in time to calc\n"
               "--inputFile <fname>:     File with input data\n"
               "--outputFile <fname>:    File to save the result \n"
               "--genInput <fname>:      Generate random input file\n"
               "--help:                  Show help\n";
  exit(1);
}

NBodyTester ProcessArgs(int argc, char** argv) {
  const char* const short_opts = "cspt:b:m:i:o:g:h";
  const option long_opts[] = {{"compare", no_argument, nullptr, 'c'},
                              {"serial", no_argument, nullptr, 's'},
                              {"parallel", no_argument, nullptr, 'p'},
                              {"threads", required_argument, nullptr, 't'},
                              {"bodyCount", required_argument, nullptr, 'b'},
                              {"momentTime", required_argument, nullptr, 'm'},
                              {"inputFile", required_argument, nullptr, 'i'},
                              {"outputFile", required_argument, nullptr, 'o'},
                              {"genInput", required_argument, nullptr, 'g'},
                              {"help", no_argument, nullptr, 'h'},
                              {nullptr, no_argument, nullptr, 0}};

  NBodyTester nbody;

  while (true) {
    const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

    if (-1 == opt) break;

    switch (opt) {
      case 'c':
        nbody.SetTestType(TestType::Compare);
        break;
      case 's':
        nbody.SetTestType(TestType::Serial);
        break;
      case 'p':
        nbody.SetTestType(TestType::Parallel);
        break;
      case 't':
        nbody.SetThreads(std::stoi(optarg));
        break;
      case 'b':
        nbody.SetBodyCount(std::stoi(optarg));
        break;
      case 'm':
        nbody.SetTimeMoment(std::stoi(optarg));
        break;
      case 'i':
        nbody.SetInputFile(std::string(optarg));
        break;
      case 'o':
        nbody.SetOutputFile(std::string(optarg));
        break;
      case 'g':
        nbody.SetTestType(TestType::GenData);
        nbody.SetOutputFile(std::string(optarg));
        break;
      case 'h':
      case '?':
      default:
        PrintHelp();
        break;
    }
  }

  return nbody;
}

int main(int argc, char* argv[]) {
  NBodyTester nbody = ProcessArgs(argc, argv);
  nbody.Run();
  return 0;
}