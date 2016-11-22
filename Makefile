CC = gcc
CXX = g++

CFLAGS = -pedantic -Wall -g
CXXFLAGS = -pedantic -Wall -g

PREFIX := /usr

all: npio_test_c npio_test_cpp example1 example2 example3 example4

% : %.c npio.h Makefile
	$(CC) -o $@ $(CFLAGS) $<
	
% : %.cpp npio.h Makefile
	$(CXX) -o $@ $(CFLAGS) $<

% : %.cc npio.h Makefile
	$(CXX) -std=c++11 -o $@ $(CFLAGS) $<

clean:
	-rm -f npio_test_c npio_test_cpp example1 example2 example3 example4 example3-out.npy example4-out.npy

test: npio_test_c npio_test_cpp example1 example2 example3 example4
	./npio_test_c
	./npio_test_cpp
	./example1
	./example2
	./example3
	./example4

install:
	install -d $(PREFIX)/include
	install -m u=rw,og=r npio.h $(PREFIX)/include/

