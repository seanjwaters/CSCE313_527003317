# makefile

# uncomment the version of the compiler that you are using
#clang++ is for Mac OS 
#C++ = clang++ -std=c++11
# g++ is for most Linux
C++ = g++ -std=c++11

all: dataserver simpleclient

reqchannel.o: reqchannel.hpp reqchannel.cpp
	$(C++) -c -g reqchannel.cpp

dataserver: dataserver.cpp reqchannel.o 
	$(C++) -o dataserver dataserver.cpp reqchannel.o -lpthread

simpleclient: simpleclient.cpp reqchannel.o
	$(C++) -o simpleclient simpleclient.cpp reqchannel.o
