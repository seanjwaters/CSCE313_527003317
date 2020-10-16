/* 
    File: semaphore.H

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 08/02/11

*/

#ifndef _semaphore_H_                   // include file only once
#define _semaphore_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <pthread.h>
#include "mutex.hpp"
#include "mutex_guard.hpp"
/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS   S e m a p h o r e  */
/*--------------------------------------------------------------------------*/

class Semaphore {
private:
friend class PCBuffer;
  /* -- INTERNAL DATA STRUCTURES
     You may need to change them to fit your implementation. */

  int             value;
  Mutex mutex;
  Mutex delay;
  pthread_mutex_t m;
  pthread_cond_t  c;

public:

  /* -- CONSTRUCTOR/DESTRUCTOR */

  Semaphore(int _val);

  ~Semaphore();

  /* -- SEMAPHORE OPERATIONS */

  int P();
  int V();
};


#endif


