#ifndef OPEN_MP_N_BODY_TESTER_H_
#define OPEN_MP_N_BODY_TESTER_H_

#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

#include "space_time.h"
#include "timer.h"

enum TestType { Compare = 0, Serial, Parallel, GenData };

class NBodyTester {
 public:
  void TestCompare();
  void TestSerial();
  void TestParallel();
  void GenData();

  void SetTestType(TestType type) { test_type_ = type; }
  void SetThreads(unsigned threads) { threads_ = threads; }
  void SetInputFile(std::string file_name) { input_file_ = file_name; }
  void SetOutputFile(std::string file_name) { output_file_ = file_name; }
  void SetBodyCount(unsigned num) { body_count_ = num; }
  void SetTimeMoment(unsigned num) { time_moment_ = num; }

  std::vector<Body> GenerateBodies(unsigned num);

  std::vector<Body> LoadState(std::string file_name);
  void SaveState(const std::vector<Body>& state, std::string file_name);

  void Run();

 private:
  const int kBodyCountDefault = 1000;
  const int kTimeMomentDefault = 10;

  TestType test_type_ = TestType::Compare;
  int threads_ = -1;
  int body_count_ = -1;
  int time_moment_ = -1;
  std::string input_file_ = "";
  std::string output_file_ = "";
};

#endif  // OPEN_MP_N_BODY_TESTER_H_