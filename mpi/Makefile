CXX=g++
MPICXX=mpic++

PARALLEL=mpi
SERIAL=serial

mpi_build: mpi_main.cpp
	$(MPICXX) $^ -o $(PARALLEL)

serial_build: main.cpp
	$(CXX) $^ -o $(SERIAL)

mpi_start:
	mpiexec -n 5 ./$(PARALLEL)
	
serial_start:
	./$(SERIAL)

clean:
	rm $(PARALLEL) $(SERIAL)