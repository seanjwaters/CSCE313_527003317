/* 
 File: my_allocator.cpp
 
 Author: Sean Waters
 Department of Computer Science
 Texas A&M University
 Date  : August 30 2020
 
 Modified:
 
 This file contains the implementation of the class MyAllocator.
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cstdlib>
#include "my_allocator.hpp"
#include "free_list.hpp"
#include <assert.h>
#include <iostream>
#include <math.h>
#include <vector>

/*--------------------------------------------------------------------------*/
/* NAME SPACES */ 
/*--------------------------------------------------------------------------*/

using namespace std;
/* I know, it's a bad habit, but this is a tiny program anyway... */

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR CLASS MyAllocator */
/*--------------------------------------------------------------------------*/

MyAllocator::MyAllocator(size_t _basic_block_size, size_t _size) {
    start = malloc(_size);

    SegmentHeader* seg1 = new(start)SegmentHeader(_size);
    seg1->CheckValid();//DB
    block_size = _basic_block_size; 

    size_t N=0;
    while( ((fib(N))*_basic_block_size)<=_size )
    {
        if(((fib(N+1))*_basic_block_size)>_size) { break; }
        ++N;
    }

    // at this point N should be the right number
    for(int i = 0 ; i<N ; ++i){
        free_list.push_back(FreeList());
    }
  
    


}

MyAllocator::~MyAllocator() {
    free(start);
}

void* MyAllocator::Malloc(size_t _length) {
    cout << "MyAllocator::Malloc called with length = " << _length << endl;
    
    // GIVEN USER-DESIRED SIZE, round up to the size you should look for in free list
    size_t templen =_length+sizeof(SegmentHeader);
    size_t len;
    int index=0;
    while(fib(index)*block_size<templen){ ++index; } //looping to get highest possible index


    if(len > remaining_memory){
        cout << "ERROR - Not enough remaining memory!" << endl;
    }

}

bool MyAllocator::Free(void* _a) {
    cout << "MyAllocator::Free called" << endl;
    // 1. CHECK IF SEGMENT HAS A BUDDY

    // 2. COALESCE 2 BUDDIES IF BUDDY IS FREE - FORM PARENT, else STOP

    // 3. REPEAT ON PARENT SEGMENT



    //std::free(_a);    
    SegmentHeader *seg = (SegmentHeader*)((char*)_a-sizeof(SegmentHeader));
    seg->CheckValid();//DB
    fl[seg->fib_index].Add(seg);

    return true;
}

bool MyAllocator::Free(void* _a) {
    cout << "MyAllocator::Free called" << endl;
    //std::free(_a);    
    SegmentHeader *seg = (SegmentHeader*)((char*)_a-sizeof(SegmentHeader));
    seg->CheckValid();//DB
    fl->Coalesce(seg);

    return true;
}

// calculate fibonacci of the number
int fib(int x){
    if(x<0){return -1;}
    if(x==0){ return 1; }
    else if (x==1){ return 2; }
    else { return (fib(x-1)-fib(x-2));}
}

