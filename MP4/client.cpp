

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#include <fstream>
#include <map>
#include "reqchannel.hpp"
#include "mutex.hpp"
#include "mutex_guard.hpp"
#include "pcbuffer.hpp"
#include "semaphore.hpp"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

typedef struct //thread tracker
{
    int rt_count;
    int wt_count;

} threadTrackr;


typedef struct //RTFArgs
{
    int n_reqs;                 // number of requests to be made by given Request Thread
    std::string patient_name;   // patient's name, to be included in the RequestChannel data request
    PCBuffer *PCB; 
    threadTrackr* trackr; 

} RTFArgs;

typedef struct //WTFArgs
{
    int nrc;
    PCBuffer *PCB;
    RequestChannel* rc;
    map<std::string, PCBuffer*> patient_map;
    threadTrackr* trackr; 

} WTFArgs;

typedef struct //STFArgs
{
    int n_reqs; 
    std::string patient_name; 
    PCBuffer *PCB; 

} STFArgs;


/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

Mutex m;

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- SUPPORT FUNCTIONS */
/*--------------------------------------------------------------------------*/




std::string int2string(int number) {
  std::stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

std::string generate_data() {
  // Generate the data to be returned to the client.
  return int2string(rand() % 100);
}

void* request_thread_func(void* args){
    std::cout << "REQUEST THREAD FUNCTION REACHED" << std::endl;
    RTFArgs* rtfa = (RTFArgs*) args;
    for(int i=0; i<rtfa->n_reqs; ++i){
        std::string req = "data " + rtfa->patient_name;
        rtfa->PCB->Deposit(req);
    }


    // now the requestthread is done.
    m.Lock();//LOCK
    rtfa->trackr->rt_count = (rtfa->trackr->rt_count)-1;
    if(rtfa->trackr->rt_count==0){
        std::cout << "\nALL request threads DONE.\n" << std::endl;
        rtfa->PCB->Deposit("done");
    }

    m.Unlock();//UNLOCK
}
std::string reply2pn(std::string reply){
    return reply.substr(5,-1);
    //example:
    //d a t a   S e a n 
    //0 1 2 3 4 5 6 7 8
}

RequestChannel rfd2rc(FDS)

void* worker_thread_func(void* args){
    /* Declaring Primitive Datatypes */
    bool done = false;
    int fd_counter = 0;

    /* Declaring Class Objects */
    WTFArgs* wtfa = (WTFArgs*) args;
    RequestChannel rc_array[wtfa->nrc];

    /*--------------------------------------------------------------- STEP 1 -  Initialization State --------------------------------------------------------------*/
    for(int i =0; i<wtfa->nrc ;++i){
        /* create rc[i] */
        std::string reply5= chan.send_request("newthread");
        RequestChannel* new_chan=new RequestChannel(reply5,RequestChannel::Side::CLIENT);
        rc_array[i]=new_chan;

        /* string request = PCB->retrieve(); */
        std::string req = wtfa->PCB->Retreive();

        /* rc[i]->send(request); */
        rc_array->send_request(request);
    }

    /*--------------------------------------------------------------- STEP 2 -  Steady State ----------------------------------------------------------------------*/
    for(;;){
        // wait for reply to come back
        FDSET fds =  ;                          // read file descs of all req channels
        FDSET afds = select(fds);               // active file descs = afds

        for(fd in afds){
            RequestChannel rc = rfd2rc(fd);     // map fd to rc
            
            // a reply is ready on ReqCh rc
            std::string reply = rc->cread();
            /*  Get patient name for reply.     *
             *  This is tricky because we don't *
             *  have the request handy.         */
            forward(reply, patient_name);

            //send next request (IF NOT DONE)
            if(!done){
                std::string req = wtfa->PCB->Retreive();
                if(req=="Done."){ 
                    done = true;
                    break;
                }
                rc->cwrite(req);
            }
        }
    }

    // now the worker thread is done.
    m.Lock();//LOCK
    wtfa->trackr->wt_count = (wtfa->trackr->wt_count)-1;
    if(wtfa->trackr->wt_count==0){
        std::cout << "ALL worker threads DONE." << std::endl;
        for(auto const& x : wtfa->patient_map){
            x.second->Deposit("done");
        }
    } else {
        wtfa->PCB->Deposit("done");
    }

    m.Unlock();//UNLOCK

}


void * statistics_thread_func(void* args){
    STFArgs* stfa = (STFArgs*) args;
    string histogram[stfa->n_reqs] = {};
    int counter = 0;
    for(;;){
        std::string req=stfa->PCB->Retrieve();
        if(req!="done"){
            histogram[counter]=req; //update histogram
            ++counter;
        }else{
            for(int i=0;i<counter;++i){ //display histogram
                std::cout<< histogram[i] <<std::endl;
            }
            break; //quit
        }
    }
}

// void create_worker(threadid, channel_id, PCBuffer*){
//     // create the wtfargs here
//     pthread_create(pthread_t *thread(&w[i]), NULL, worker_thread_func, *WTFArgs);
// }



/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/


int main(int argc, char * argv[]) {

    if(fork()==0){
        execv("dataserver",NULL);
        std::cout << "CLIENT STARTED:" << std::endl;
    }
    struct timeval t1, t2, n1, n2;

    //threads

    int opt_int=0;
    int nreq; //number of request per patient
    int size; //size of bounded buffer
    int nrc; //number of request channels to be made by single worker thread
    while( (opt_int = getopt(argc,argv, "n:b:w:")) != -1 )
    {
        switch(opt_int)
            {
                case 'n':
                    nreq = atoi(optarg);
                    //cout << optarg << endl;
                    break;
                case 'b':
                    size = atoi(optarg);
                    //cout << optarg << endl;
                    break;
                case 'w':
                    nrc = atoi(optarg);
                    //cout << optarg << endl;
                    break;
                default:
                    fprintf(stderr, "getopt");
                    break;
            }
    }


    std::cout << "Establishing control channel... " << std::flush;
    RequestChannel chan("control", RequestChannel::Side::CLIENT);
    std::cout << "done." << std::endl;
    // std::cout << "CONTROL CHANNEL DONE DONE" << std::endl; //DEBUG
    PCBuffer PCB(size);

    std::string *patient_names = new std::string[3]; 
    patient_names[0] = "Joe Smith";
    patient_names[1] = "Jane Smith";
    patient_names[2] = "John Doe";
    
    threadTrackr* trackr = new threadTrackr;
    trackr->rt_count=3;
    trackr->wt_count=1;

    map<string, PCBuffer*> patient_map;
    for(int i=0 ; i<3 ; ++i){
        PCBuffer *stat_buffer = new PCBuffer(size);
        patient_map[patient_names[i]]=stat_buffer;
    }

    pthread_t *req_threads = new pthread_t[3]; 
    pthread_t *worker_threads = new pthread_t[1];
    pthread_t *statistics_threads = new pthread_t[3];
    

    gettimeofday(&t1,NULL);
    /*                CREATING REQUEST THREADS                      */
    for(int i=0; i<3; ++i){
        // std::cout << "CREATING REQUEST THREAD NUMBER " << i << std::endl; //DEBUG
        RTFArgs* args = new RTFArgs;
        args->n_reqs = nreq;
        args->patient_name = patient_names[i];
        args->PCB = &PCB;
        args->trackr=trackr;
        pthread_create(&req_threads[i],NULL,request_thread_func,args);
    }

    /*                CREATING WORKER THREAD                      */

    WTFArgs *args = new WTFArgs;
    args->rc=&chan;
    args->PCB=&PCB;
    args->nrc=nrc;
    args->patient_map=patient_map;
    args->trackr=trackr;
    pthread_create(&worker_threads[0],NULL,worker_thread_func,args);
    /*--------------------------------------------------------------*/
    

    /*                CREATING STATISTICS THREADS                      */
    for(int i=0; i<3; ++i){
        STFArgs *args = new STFArgs;
        args->n_reqs=nreq;
        args->patient_name=patient_names[i];
        args->PCB=patient_map[patient_names[i]];
        pthread_create(&statistics_threads[i],NULL,statistics_thread_func,args);
    }
    gettimeofday(&t2,NULL);
    std::cout << "Average time taken by server to make " << nreq << " requests:    " << (t2.tv_usec-t1.tv_usec)/nreq << " milliseconds/request" << std::endl;


    
    /*  pthread_join for all Threads */
    for(int i=0;i<3;++i){
        pthread_join(req_threads[i],NULL);
    }
    std::cout << "\nRequest Threads all done and closed\n" << std::endl; //DEBUG
    
    for(int i=0;i<1;++i){
        pthread_join(worker_threads[i],NULL);
    }
    std::cout << "\nWorker Threads all done and closed\n" << std::endl; //DEBUG
    
    for(int i=0;i<3;++i){
        pthread_join(statistics_threads[i],NULL);
    }
    std::cout << "\nStatistics Threads all done and closed\n" << std::endl; //DEBUG
    
    std::string reply4 = chan.send_request("quit");
    std::cout << "Reply to request 'quit' is '" << reply4 << "'" << std::endl;


    
    usleep(1000000);
}


