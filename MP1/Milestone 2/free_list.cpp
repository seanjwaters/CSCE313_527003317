/* 
    File: free_list.cpp

    Author: Sean Waters
            Department of Computer Science and Engineering
            Texas A&M University
    Date  : September 6 2020jn

    Modified: 

    This file contains the implementation of the class FreeList.

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <iostream>
#include "free_list.hpp"
#include <cassert>

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
/* FUNCTIONS FOR CLASS SegmentHeader */
/*--------------------------------------------------------------------------*/

SegmentHeader::SegmentHeader(size_t _length, bool _is_free) {
  length = _length;                     //size of data segment
  is_free = _is_free;                   //boolean for whether SH is free
  cookie = COOKIE_VALUE;
  SegmentHeader* next = NULL;
  SegmentHeader* prev = NULL;
  
}

SegmentHeader::~SegmentHeader() {
  // You may need to add code here.
}


void SegmentHeader::CheckValid() {
  if (cookie != COOKIE_VALUE) {
    cout << "INVALID SEGMENT HEADER!!" << endl;
    assert(false);
    // You will need to check with the debugger to see how we got into this
    // predicament.
  }
}

SegmentHeader* SegmentHeader::Split(size_t _len){
    SegmentHeader* seg_new = new((void*)((char*)(this+_len)))SegmentHeader(this->length-_len);
    this->length = _len;
    return seg_new;
} 


/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR CLASS FreeList */
/*--------------------------------------------------------------------------*/

FreeList::FreeList() {
  head = NULL;
  tail = NULL;
}

FreeList::~FreeList() {
  // You may need to add code here.
}

bool FreeList::Add(SegmentHeader * _segment) {
  cout << "Add called." << endl;
  PP();
  
  if(head==NULL){
    head = _segment;
    tail = _segment;
    PP();
    return true;
  }

  _segment->next = head;

  if(head!=NULL){
    head->prev=_segment;
  }
  head = _segment;
  _segment->is_free=true;

  PP();
  return true;
}


bool FreeList::Remove(SegmentHeader * _segment) {
  cout << "Remove called." << endl;
  PP();

  // IF LIST EMPTY
  if(head==NULL || _segment==NULL){
    printf("Attempting to remove from empty list.");
    return false;
  }

  // IF HEAD IS THE SEGMENT => 1) and THERES MORE THAN 1 ELEMENT, 2) THERES ONLY THE 1 ELEMENT
  if(head ==_segment){
    if(head->next==NULL){

    }
    head = _segment->next;
    return true;
  }

  if(_segment->next != NULL){
    _segment->prev->next = _segment->next;
  }

  if(_segment->prev != NULL){
    _segment->prev->next = _segment->next;
  }




  SegmentHeader* seg = head;
  while(seg!=_segment){
    seg = seg->next;
  }
  if(seg==_segment){
    seg->prev->next=seg->next;
  }

  _segment->is_free=false;
  return true;
}

void FreeList::PP(){
  SegmentHeader* current = head;
  int n = 1;
  while(current!=NULL){
    cout <<  "SH " << n << endl;
    cout << "Length: " << (current->length) << endl;
    cout << "Cookie: " <<  (current->cookie) << endl;
    current=current->next;
    n++;
  }
}
