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

    block_size = _basic_block_size; 

    size_t N=0;
    while( ((fib(N))*block_size)<=_size )
    {
        if(((fib(N+1))*block_size)>_size) { break; }
        ++N;
    }

    // at this point N should be the highest index value
    for(int i = 0 ; i<=N ; ++i){
        fl.push_back(FreeList());
    }

    start = malloc(fib(N)*block_size);
    SegmentHeader *root_seg = new(start)SegmentHeader(fib(N)*block_size);
    fl[N].Add(root_seg);

    max_flist_index = N;
    // printf("size about to be used: %d\n", fib(N)*block_size);
    // cout << "Test" << endl;
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
    
    len = fib(index)*block_size;

  
    return MallocTwo(len, index);
}


void* MyAllocator::MallocTwo(size_t len, int orig_index){
    int index = orig_index;
    while( (index<=max_flist_index)&&((fl[index].empty())||(fib(index)*block_size<len)) ) { ++index; }

    if(index>max_flist_index){ return nullptr; } // FAIL

    SegmentHeader *seg1 = fl[index].head;
    SegmentHeader *seg2;    


    fl[index].Remove(fl[index].head);

    if(seg1->length==len){ //SUCCESS
        void *ptr = (void*)((char*)seg1+sizeof(SegmentHeader));
        return ptr;
    } else{
        seg2 = seg1->Split(len);
        fl[index-1].Add(seg1);
        fl[index-2].Add(seg2);
        return MallocTwo(len,index);
    }

    
}

bool MyAllocator::Free(void* _a) {
    // 1. CHECK IF SEGMENT HAS A BUDDY
    // 2. COALESCE 2 BUDDIES IF BUDDY IS FREE - FORM PARENT, else STOP
    // 3. REPEAT ON PARENT SEGMENT
    cout << "MyAllocator::Free called" << endl;
    SegmentHeader *seg = (SegmentHeader*)((char*)_a-sizeof(SegmentHeader));
    return Coalesce(seg);

}

bool MyAllocator::Coalesce(SegmentHeader *seg){
    int seg_idx = seg->fib_index;
    SegmentHeader *temp_seg;
    SegmentHeader *mseg;
    SegmentHeader *bbseg;
    SegmentHeader *sbseg;
    FreeList *temp_list;

    if(seg->buddy_type==SegmentHeader::buddy::LEFT){    //if you are big buddy (L segment) check if right buddy is free
    // search for segment in the Freelist[fib_index-1] that has starting address _segment+(_segment->length)
    size_t sb_len = fib(seg_idx-1)*block_size;
    temp_seg = seg+seg->length;


        if((temp_seg->fib_index==seg_idx-1)&&(temp_seg->length==sb_len)){ //IF buddy is correct size 
            bbseg = min(seg,temp_seg);
            sbseg = max(seg,temp_seg);
            mseg = bbseg;
            mseg->fib_index=bbseg->fib_index+1;
            mseg->buddy_type=sbseg->inheritance;
            mseg->inheritance = bbseg->inheritance;
            Coalesce(mseg);
        } else {
            fl[seg_idx].Add(seg);
            return true;
        }

    } else if(seg->buddy_type==SegmentHeader::buddy::RIGHT){    //else if small buddy (R segment) check if left buddy is free
    size_t bb_len = fib(seg_idx+1)*block_size;
    temp_seg = seg-bb_len;
        if((temp_seg->fib_index==seg_idx+1)&&(temp_seg->length==bb_len)){ //IF buddy is correct size 
            bbseg = min(seg,temp_seg);
            sbseg = max(seg,temp_seg);
            mseg = bbseg;
            mseg->fib_index=bbseg->fib_index+1;
            mseg->buddy_type=sbseg->inheritance;
            mseg->inheritance = bbseg->inheritance;
            Coalesce(mseg);
        } else {
            fl[seg_idx].Add(seg);
            return true;
        }
    
    } else if(seg->fib_index==max_flist_index){
        fl[max_flist_index].Add(seg);
        return true;
    }

}


int MyAllocator::fib(int x){ // calculate fibonacci of the number
    if(x<0){return -1;}
    if(x==0){ return 1; }
    else if (x==1){ return 2; }
    else { return (fib(x-1)-fib(x-2));}
}

