# makefile

# uncomment the version of the compiler that you are using
# clang++ is for Mac OS 
# C++ = clang++
# g++ is for most Linux
 C++ = g++

all: memtest

clean:
	rm -f *.o

my_allocator.o : my_allocator.hpp my_allocator.cpp
	$(C++) -std=c++11 -c -g my_allocator.cpp

free_list.o : free_list.hpp free_list.cpp
	$(C++) -std=c++11 -c -g free_list.cpp

memtest: memtest.cpp my_allocator.o free_list.o
	$(C++) -std=c++11 -o memtest memtest.cpp my_allocator.o free_list.o

