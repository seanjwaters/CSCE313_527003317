/* 
    File: free_list.cpp

    Author: Sean Waters
            Department of Computer Science and Engineering
            Texas A&M University
    Date  : September 6 2020

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

SegmentHeader::SegmentHeader(size_t _length) {
  length = _length;                     //size of data segment
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

    seg_new->buddy_type = buddy::RIGHT;
    seg_new->inheritance = this->buddy_type;
    seg_new->fib_index = this->fib_index-2;

    this->length = _len;
    this->buddy_type = buddy::LEFT;
    this->fib_index = this->fib_index-1;

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


bool FreeList::empty(){
if(this->head==NULL){ return true; }

return false;
}
  

// Adding at the front
bool FreeList::Add(SegmentHeader * _segment) {
  // cout << "Add called." << endl;
  // PP();
  if(head==NULL){
    head=_segment;
    tail=_segment;
    _segment->next=NULL; //just sticking nulls where they should go just in case
    _segment->prev=NULL;
    return true;
  }

  _segment->prev = NULL;
  _segment->next = head;
  head->prev=_segment;
  head=_segment;


  _segment->is_free=true;
  return true;
}
/*
// Adding at the back
bool FreeList::Add(SegmentHeader * _segment) {
  // cout << "Add called." << endl;
  // PP();
  _segment->CheckValid();
  if(head==NULL || tail==NULL){
    head = _segment;
    tail = _segment;
    return true;
  }
  _segment->next = NULL;
  _segment->prev = tail;
  tail->next = _segment;
  tail=_segment;

  _segment->is_free=true;

  return true;
}
*/

bool FreeList::Remove(SegmentHeader * _segment) {
  // cout << "Remove called." << endl;
  // PP();
  _segment->CheckValid();

  // List is empty
  if(head==NULL || tail==NULL){
    return false;
  }

  // List contains only one element
  if(head==tail){
      head=NULL;
      tail=NULL;
      return true;
  }
  // Segment is at the beginning
  if(_segment==head){
    head = head->next;
    head->prev = NULL;
    return true;
  }
  // Segment is at the end
  if(_segment==tail){
    tail = tail->prev;
    tail->next = NULL;
    return true;
  }
  // Segment is at other position
  SegmentHeader* current = head;
  while(current!=_segment){
    current=current->next;
  }
  if(current->next!=NULL){
    current->next->prev=current->prev;
  }

  current = NULL;

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
