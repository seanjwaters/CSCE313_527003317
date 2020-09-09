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
#include <assert.h>
#include <iostream>

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
    current_start = start;
    remaining_memory = _size;
    
}

MyAllocator::~MyAllocator() {
    free(start);
}

void* MyAllocator::Malloc(size_t _length) {
    // This empty implementation just uses C standard library malloc
    cout << "MyAllocator::Malloc called with length = " << _length << endl;

    if(_length > remaining_memory){
        cout << "ERROR - Not enough remaining memory!" << endl;
    }
    void *res = current_start;
    current_start = (char*)current_start + _length;
    remaining_memory -= _length;

    return res;
    //return std::malloc(_length);
}

bool MyAllocator::Free(void* _a) {
    // This empty implementation just uses C standard library free
    cout << "MyAllocator::Free called" << endl;
    //std::free(_a);
    return true;
}

