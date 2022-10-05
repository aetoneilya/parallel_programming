CXX=g++
NVCC=nvcc
# CXXFLAGS=-pedantic -Wall -Wextra -Wshadow -Wconversion -Wunreachable-code -std=c++11


EXECUTABLE=radix

.PHONY: all build clean rebuild

all: build

build: src/main.cu src/radix.cu
		$(NVCC) src/main.cu src/radix.cu $(CXXFLAGS) -o $(EXECUTABLE)

clean:
		rm -rf $(MAINOBJ) $(OBJ) $(EXECUTABLE)