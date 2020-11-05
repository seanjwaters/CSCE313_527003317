
/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <pthread.h>
#include "semaphore.hpp"

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

Semaphore::Semaphore(int _val){ 
    mutex.Unlock();
    if(_val==0){
        delay.Lock();
    }else{ delay.Unlock(); }
    value = _val; 
    }

Semaphore::~Semaphore(){

}



int Semaphore::P(){
    delay.Lock();
    { MutexGuard mg(mutex);

    --value;
    if(value>0)
        { delay.Unlock();}

    }


}

int Semaphore::V(){
    
    { MutexGuard mg(mutex);

    ++value;
    if(value==1)
        {delay.Unlock();}

    }
}





