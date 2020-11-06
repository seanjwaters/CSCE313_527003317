# makefile

# uncomment the version of the compiler that you are using
#clang++ is for Mac OS 
#C++ = clang++ -std=c++11
# g++ is for most Linux
C++ = g++ -std=c++11

all: test

clean:
	rm -f *.o

test: test.cpp
	$(C++) -g -o a test.cpp
