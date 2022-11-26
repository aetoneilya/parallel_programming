#include <CL/cl.h>

#include <iterator>
#include <string>
#include <vector>

#include "timer.h"

#define MAX_SOURCE_SIZE (0x100000)

int findLargestSequenceSerial(int N);
int findLargestSequenceParallel(int N);

int sequenceLen(int n);
