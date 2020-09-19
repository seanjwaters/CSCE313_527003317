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

    //free_list = new FreeList(_size); // this was how i initially constructed the free list
    seg1->CheckValid();//DB
    free_list.Add(seg1);
    

    size_t N=1;


    FreeList *fl = malloc(N*sizeof(FreeList));


}

MyAllocator::~MyAllocator() {
    free(start);
}

void* MyAllocator::Malloc(size_t _length) {
    // This empty implementation just uses C standard library malloc
    cout << "MyAllocator::Malloc called with length = " << _length << endl;
    
    //rounding up
    size_t templen =_length+sizeof(SegmentHeader);
    size_t len;
    if((templen%block_size)==0){
        len = templen;
    } else {
        len = ((templen/block_size)+1)*block_size;
    }


    //big picture way of checking if theres enough memory before allocating
    if(len > remaining_memory){
        cout << "ERROR - Not enough remaining memory!" << endl;
    }

    //checking freelist for spot thats big enough for _length
    SegmentHeader* seg = free_list.head;
    seg->CheckValid();//DB
    while( (seg!=NULL)&&(seg->length<len) ){
        seg->CheckValid();//DB
        seg=seg->next;
    }
    
    if(seg==NULL) { return NULL; }

    free_list.Remove(seg);
    if(seg->length>len){
        SegmentHeader* seg2 = seg->Split(len);
        seg2->CheckValid();//DB
        free_list.Add(seg2);
    }
    void *ptr = (void*)((char*)seg+sizeof(SegmentHeader));

    return ptr;
}

bool MyAllocator::Free(void* _a) {
    cout << "MyAllocator::Free called" << endl;
    //std::free(_a);    
    SegmentHeader *seg = (SegmentHeader*)((char*)_a-sizeof(SegmentHeader));
    seg->CheckValid();//DB
    free_list.Add(seg);

    return true;
}

// calculate fibonacci of the number
int fib(int x){
    if(x=-0){ return 1; }
    else if (x==1){ return 2; }
    else { return (fib(x-1)-f(x-2));}
}

