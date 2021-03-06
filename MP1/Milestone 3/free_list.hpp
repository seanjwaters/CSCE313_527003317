/* 
    File: free_list.hpp

    Author: Sean Waters
            Department of Computer Science and Engineering
            Texas A&M University
    Date  : September 6 2020

    Modified:

*/

#ifndef _free_list_hpp_                   // include file only once
#define _free_list_hpp_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cstdlib>

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

class SegmentHeader {
friend class FreeList;
friend class MyAllocator;
private:

  static const unsigned COOKIE_VALUE = 0xBADB00;
  unsigned int cookie; /* To check whether this is a genuine header! */
  size_t length;
  bool is_free;
  SegmentHeader* next;
  SegmentHeader* prev;


  int fib_index;
  enum class buddy {LEFT,
                    RIGHT,
                    ORIGIN};
  buddy buddy_type;
  buddy inheritance;


public:
  

  SegmentHeader(size_t _length);
  
  ~SegmentHeader();
  /* We probably won't need the destructor. */


  SegmentHeader* Split(size_t _len);

  void CheckValid();
  /* Check if the cookie is valid. */
};

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS  FreeList */
/*--------------------------------------------------------------------------*/

class FreeList {
friend class MyAllocator;
friend class SegmentHeader;
private:

  SegmentHeader* head;
  SegmentHeader* tail;

public:



  FreeList(); 
  /* This function initializes a new free-list. */

  ~FreeList(); 
  /* We probably don't need a destructor. */ 

  bool empty();

  bool Remove(SegmentHeader * _segment); 
  /* Remove the given segment from the given free list. 
     Returns true if the function succeeds.
  */ 

  bool Add(SegmentHeader * _segment); 
  /* Add the segment to the given free list. */

  void PP();
  /*Pretty print the whole free list*/
  
};

#endif 
