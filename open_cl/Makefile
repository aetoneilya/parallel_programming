CXX=g++
LDFLAGS=-I/usr/local/cuda/include/ -L/usr/local/cuda/lib64/ -lOpenCL -std=c++11

EXECUTABLE=sequence

.PHONY: all build clean rebuild

all: build

build: src/main.cpp src/sequence.cpp
	$(CXX) $(LDFLAGS) $^ -o $(EXECUTABLE)

clean:
	rm -rf  $(EXECUTABLE)

rebuild: clean all