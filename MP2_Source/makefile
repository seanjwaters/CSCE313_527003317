# makefile

# uncomment the version of the compiler that you are using
#clang++ is for Mac OS 
# C++ = clang++ -std=c++11
# g++ is for most Linux
C++ = /usr/bin/g++ -std=c++11

.DEFAULT_GOAL := all

clean:
	rm -f *.o sync_test_mutex sync_test_mg sync_test_semaphore sync_test_pcbuffer

all: test_suite

reqchannel.o: reqchannel.hpp reqchannel.cpp
	$(C++) -c -g reqchannel.cpp

mutex.o: mutex.hpp mutex.cpp
	$(C++) -c -g mutex.cpp -lpthread

mutex_guard.o: mutex_guard.hpp mutex_guard.cpp
	$(C++) -c -g mutex_guard.cpp -lpthread


test_suite: sync_test_mutex sync_test_mg

sync_test_mutex: sync_test.cpp mutex.o 
	$(C++) -DMUTEX_TEST -o sync_test_mutex sync_test.cpp mutex.o -lpthread

sync_test_mg: sync_test.cpp mutex.o mutex_guard.o
	$(C++) -DMUTEX_GUARD_TEST -o sync_test_mg sync_test.cpp mutex.o mutex_guard.o -lpthread

