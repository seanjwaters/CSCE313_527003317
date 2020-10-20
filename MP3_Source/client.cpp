/* 
    File: client.cpp

    Author: Sean Waters
    Date  : 2020/10/19

*/

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
#include "mutex.cpp"
#include "mutex_guard.cpp"
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
    PCBuffer *PCB;
    std::string patient_name; 
    RequestChannel* rc;
    int thread_id;
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
    RTFArgs* rtfa = (RTFArgs*) args;
    for(int i=0; i<rtfa->n_reqs; ++i){
        std::string req = "data " + rtfa->patient_name;
        rtfa->PCB->Deposit(req);
    }


    // now the requestthread is done.
    m.Lock();//LOCK
    rtfa->trackr->rt_count = (rtfa->trackr->rt_count)-1;
    if(rtfa->trackr->rt_count==0){
        std::cout << "ALL request threads DONE." << std::endl;
        rtfa->PCB->Deposit("done");
    }

    m.Unlock();//UNLOCK
}


void* worker_thread_func(void* args){
    
    WTFArgs* wtfa = (WTFArgs*) args;
    for(;;){
        
        std::string req = wtfa->PCB->Retrieve();  // take request from PCB
        std::cout << "Processing string '" << req << "'" << std::endl;    // print request
        if(req=="done"){
            std::cout << "Worker Thread " << wtfa->thread_id << " complete." << std::endl;
            // std::cout << "STRING ABOUT TO BE REQUESTED: " << req << std::endl; //DEBUG CODE
            delete wtfa->rc;
            break;
        }
        std::string PATIENT_NAME= req.substr(5,-1);
        // std::cout << "STRING ABOUT TO BE REQUESTED: " << req << std::endl; //DEBUG CODE
        std::string result = wtfa->rc->send_request(req);    // send request to dataserver using WTFArgs->rc
        std::cout << "Reply to request '" << req << "' = "<< result << std::endl; // print response
        wtfa->patient_map[PATIENT_NAME]->Deposit(result);
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
        exit(0);
    }
    struct timeval t1, t2, n1, n2;

    //threads

    int opt_int=0;
    int nreq; //number of request per patient
    int size; //size of bounded buffer
    int nwt; //number of worker threads
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
                    nwt = atoi(optarg);
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
    
    PCBuffer PCB(size);

    std::string *patient_names = new std::string[nwt]; 
    patient_names[0] = "Joe Smith";
    patient_names[1] = "Jane Smith";
    patient_names[2] = "John Doe";
    
    threadTrackr* trackr;
    trackr->rt_count=nwt;
    trackr->wt_count=nwt;

    map<string, PCBuffer*> patient_map;
    for(int i=0 ; i<nwt ; ++i){
        PCBuffer *stat_buffer = new PCBuffer(size);
        patient_map[patient_names[i]]=stat_buffer;
    }

    pthread_t *req_threads = new pthread_t[nwt]; 
    pthread_t *worker_threads = new pthread_t[nwt];
    pthread_t *statistics_threads = new pthread_t[nwt];
    

    gettimeofday(&t1,NULL);
    for(int i=0; i<nwt; ++i){ //CREATING REQUEST THREADS
        RTFArgs* args = new RTFArgs;
        args->n_reqs = nreq;
        args->patient_name = patient_names[i];
        args->PCB = &PCB;
        args->trackr=trackr;
        pthread_create(&req_threads[i],NULL,request_thread_func,&args);
    }
    
    for(int i=0; i<nwt; ++i){ //CREATING WORKER THREADS
        //create a channel for every worker thread
        //send a newthread request to the dataserver using control RC, this will respond with the new channel name new_chan
        std::string reply5= chan.send_request("newthread");
        std::cout << "Reply to request 'newthread' is " << reply5 << "" << std::endl;
        RequestChannel* new_chan=new RequestChannel(reply5,RequestChannel::Side::CLIENT);

        WTFArgs *args = new WTFArgs;
        args->rc=new_chan;
        args->PCB=&PCB;
        args->thread_id=1;
        args->patient_map=patient_map;
        args->trackr=trackr;
        args->thread_id=pthread_create(&worker_threads[i],NULL,worker_thread_func,&args);
        
    }

    for(int i=0; i<nwt; ++i){ //CREATING STATISTICS THREADS
        STFArgs *args = new STFArgs;
        args->n_reqs=nreq;
        args->patient_name=patient_names[i];
        args->PCB=patient_map[patient_names[i]];
        pthread_create(&statistics_threads[i],NULL,statistics_thread_func,&args);
    }
    gettimeofday(&t2,NULL);
    std::cout << "Average time taken by server to make " << nreq << " requests:    " << (t2.tv_usec-t1.tv_usec)/nreq << " milliseconds/request" << std::endl;

    
    std::ofstream client_data;
    std::ofstream server_data;
    client_data.open("client_data.csv");
    server_data.open("server_data.csv");
    



    std::string reply4 = chan.send_request("quit");
    std::cout << "Reply to request 'quit' is '" << reply4 << "'" << std::endl;


    
    usleep(1000000);
}


