# makefile

# uncomment the version of the compiler that you are using
#clang++ is for Mac OS 
#C++ = clang++ -std=c++11
# g++ is for most Linux
C++ = g++ -std=c++11

all: dataserver client

clean:
	rm -f *.o

reqchannel.o: reqchannel.hpp reqchannel.cpp
	$(C++) -c -g reqchannel.cpp

pcbuffer.o: pcbuffer.cpp pcbuffer.hpp semaphore.o
	$(C++) -c -g pcbuffer.cpp

semaphore.o: semaphore.cpp semaphore.hpp
	$(C++) -c -g semaphore.cpp

dataserver: dataserver.cpp reqchannel.o 
	$(C++) -g -o dataserver dataserver.cpp reqchannel.o -lpthread

client: client.cpp reqchannel.o pcbuffer.o semaphore.o
	$(C++) -g -o client client.cpp reqchannel.o pcbuffer.o semaphore.o -lpthread
