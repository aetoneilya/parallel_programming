#ifndef SORT_TIMER_H_
#define SORT_TIMER_H_

#include <time.h>

class Timer {
 public:
  void Start() {
    clock_gettime(CLOCK_REALTIME, &start_);
    clock_gettime(CLOCK_REALTIME, &end_);
  }
  void End() { clock_gettime(CLOCK_REALTIME, &end_); }

  long int GetNanosec() {
    return 1000000000 * (end_.tv_sec - start_.tv_sec) +
           (end_.tv_nsec - start_.tv_nsec);
  }

  long int GetSecond() { return end_.tv_sec - start_.tv_sec; }

 private:
  struct timespec start_, end_;
};

#endif  // SORT_TIMER_H_