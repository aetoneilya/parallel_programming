CXX=g++
CXXFLAGS=-c -pedantic -Wall -Wextra -Wshadow -Wconversion -Wunreachable-code -std=c++11 -fopenmp
LDFLAGS=-fopenmp

MAINSRC=src/main.cpp
MAINOBJ=$(MAINSRC:.cpp=.o)
SRC=$(filter-out $(MAINSRC), $(shell find src -type f -name "*.cpp"))
OBJ=$(SRC:.cpp=.o)

EXECUTABLE=nbody

.PHONY: all build clean rebuild

all: build

build: $(MAINOBJ) $(OBJ)
	$(CXX) $^ -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf $(MAINOBJ) $(OBJ) $(EXECUTABLE)

rebuild: clean all