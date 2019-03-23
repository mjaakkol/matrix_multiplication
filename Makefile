#CFLAGS=-Wall -std=c++11 -lthread
CFLAGS=-Wall -std=c++11
EXE=matrix_test


$(EXE): matrix_test.o
	g++ $(CFLAGS) -o $(EXE) matrix_test.o

matrix_test.o: matrix_test.cpp matrix.hpp test_vectors.hpp
	g++ $(CFLAGS) -c matrix_test.cpp -o matrix_test.o

all: $(EXE) docs

docs: matrix.hpp Doxyfile
	Doxygen

clean:
	rm -rf *.o $(EXE)
	rm -rf html

