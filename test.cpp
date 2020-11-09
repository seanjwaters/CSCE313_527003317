#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <pthread.h>
#include <algorithm>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#include <fstream>
#include <map>
using namespace std;

int main(){
    fd_set test;
    FD_ZERO(&test);
    int arr[10]={1,2,3,4,5,6,7,8,9,10};

    int maxfd = 0;
    // FD_SET(0,&test);
    // FD_SET(1,&test);
    for(int i=0; i<10 ; ++i){
        FD_SET(arr[i], &test);
        // maxfd = max(maxfd, test[i]);
    }


}