#ifndef COMMON_COMMON_H_
#define COMMON_COMMON_H_

#include <fstream>
#include <iostream>

#include "sequence.h"
#include "timer.h"

enum TestType { Compare = 0, Serial, Parallel };

struct TestFlags {
  TestType test_type = TestType::Compare;
  int work_group_size = 64;
  int N = 1024;
  bool out_save = false;
};

void outSequence(size_t n) {
  std::cout << "N: " << n << " len: " << sequenceLen(n) << std::endl;
  while (n > 1) {
    std::cout << n << "->";
    n = n % 2 == 0 ? n / 2 : 3 * n + 1;
  }
  std::cout << n << std::endl;
}

void startTimeTest(TestFlags flags) {
  Timer timer;
  TestType type = flags.test_type;
  int N = flags.N;
  std::cout << "N: " << N << std::endl;

  int parallel_res, serial_res;
  long int serial_time, parallel_time;
  if (type == TestType::Compare || type == TestType::Serial) {
    timer.Start();
    serial_res = findLargestSequenceSerial(N);
    timer.End();
    serial_time = timer.GetNanosec();
    std::cout << "Serial   : " << timer.GetSecond() << " " << timer.GetNanosec()
              << std::endl;
  }

  if (type == TestType::Compare || type == TestType::Parallel) {
    timer.Start();
    parallel_res = findLargestSequenceParallel(N);
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

    std::cout << "parallel: " << parallel_res << std::endl
              << "serial: " << serial_res << std::endl;

    if (parallel_res == serial_res)
      std::cout << "Success!\n";
    else
      std::cout << "Result dont match!\n";
  }

  if (flags.out_save) outSequence(serial_res);
};

#endif