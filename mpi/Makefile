CXX=g++
MPICXX=mpic++

PARALLEL=mpi
SERIAL=serial

all: mpi_build serial_build

mpi_build: mpi_main.cpp
	$(MPICXX) $^ -o $(PARALLEL) -std=c++11

serial_build: main.cpp
	$(CXX) $^ -o $(SERIAL) -std=c++11

mpi_start:
	mpiexec -n 5 ./$(PARALLEL)
	
serial_start:
	./$(SERIAL)

clean:
	rm $(PARALLEL) $(SERIAL)