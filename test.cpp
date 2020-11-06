#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#include <fstream>
#include <map>
using namespace std;

int main(){
    int int_array;
    int *int_parray = {1,2,3,4,5};
    // for(int i=0;i<5;++i){
    //     int_array[i]=(i*2);
    // }
    for(int i=0;i<5;++i){
        cout << *int_parray[i];
    }
    


}