/* 
    File: mutex.cpp

    Author: Sean Waters
            Department of Computer Science
            Texas A&M University
    Date  : 09/23/19

*/


/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <pthread.h>
#include "mutex.hpp"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */


Mutex::Mutex(){
  m = PTHREAD_MUTEX_INITIALIZER;
}

Mutex::~Mutex(){
  pthread_mutex_destroy(&m);
}

void Mutex::Lock(){
  pthread_mutex_lock(&m);
}

void Mutex::Unlock(){
  pthread_mutex_unlock(&m);
}



