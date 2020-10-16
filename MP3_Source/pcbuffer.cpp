/* 
    File: pcbuffer.hpp

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2020/09/27

*/


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <string>
#include <pthread.h>
#include "pcbuffer.hpp"

using namespace std;
/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS   bounded P C B u f f e r  */
/*--------------------------------------------------------------------------*/

PCBuffer::PCBuffer(int _size):size(_size),mutex(1),full(0),empty(_size){}

PCBuffer::~PCBuffer(){}
  


int PCBuffer::Deposit(string _item){
    empty.P();
    mutex.P();
    buffer.push_back(_item);
    mutex.V();
    full.V();
}

string PCBuffer::Retrieve(){
    full.P();
    mutex.P();
    string s = buffer.back();
    buffer.pop_back();
    mutex.V();
    empty.V();
    return s;
}






