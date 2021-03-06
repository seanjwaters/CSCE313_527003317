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


void* worker_thread_func(void* args){
    
    WTFArgs* wtfa = (WTFArgs*) args;
    for(;;){
        
        std::string req = wtfa->PCB->Retrieve();  // take request from PCB
        std::cout << "\nProcessing string '" << req << "'" << std::endl;    // print request
        if(req=="done"){
            std::cout << "Worker Thread " << wtfa->thread_id << " complete." << std::endl;
            // std::cout << "STRING ABOUT TO BE REQUESTED: " << req << std::endl; //DEBUG CODE

            /* FOR GRADER - I wasn't having the WT request chanels send a "quit" request. So I added that.*/
            std::string reply = wtfa->rc->send_request("quit");
            delete wtfa->rc;
            break;
        }
        std::string PATIENT_NAME= req.substr(5,-1);
        // std::cout << "STRING ABOUT TO BE REQUESTED: " << req << std::endl; //DEBUG CODE
        std::string result = wtfa->rc->send_request(req);    // send request to dataserver using WTFArgs->rc
        std::cout << "\nReply to request '" << req << "' = "<< result << std::endl; // print response
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
        // exit(0); // FOR GRADER - this line (line 206) should not have been here.
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
    // std::cout << "CONTROL CHANNEL DONE DONE" << std::endl; //DEBUG
    PCBuffer PCB(size);

    std::string *patient_names = new std::string[3]; 
    patient_names[0] = "Joe Smith";
    patient_names[1] = "Jane Smith";
    patient_names[2] = "John Doe";
    
    threadTrackr* trackr = new threadTrackr;
    trackr->rt_count=3;
    trackr->wt_count=nwt;

    map<string, PCBuffer*> patient_map;
    for(int i=0 ; i<3 ; ++i){
        PCBuffer *stat_buffer = new PCBuffer(size);
        patient_map[patient_names[i]]=stat_buffer;
    }

    /*  FOR GRADER - (lines 263-265) Initially, I had made the # of RThreads & SThreads equal to the # of WThreads.
        This was wrong, and I've now hard-coded the number of RT and ST to 3, which is the number of patients for this MP.
        This also means I had to edit where I was originally using nwt (e.g. in the for loops which create the threads). */
    pthread_t *req_threads = new pthread_t[3]; 
    pthread_t *worker_threads = new pthread_t[nwt];
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

    /*                CREATING WORKER THREADS                      */
    for(int i=0; i<nwt; ++i){
        // std::cout << "CREATING WORKER NUMBER " << i << std::endl; //DEBUG

        //create a channel for every worker thread
        //send a newthread request to the dataserver using control RC, this will respond with the new channel name new_chan
        std::string reply5= chan.send_request("newthread");
        std::cout << "Reply to request 'newthread' is " << reply5 << "" << std::endl;
        RequestChannel* new_chan=new RequestChannel(reply5,RequestChannel::Side::CLIENT);

        // std::cout << "WORKER CHANNEL NUMBER " << i << std::endl; //DEBUG

        WTFArgs *args = new WTFArgs;
        args->rc=new_chan;
        args->PCB=&PCB;
        /* FOR GRADER - (line 296) thread_id was initialized to 1. This was a mistake & should be i.*/
        args->thread_id=i;
        args->patient_map=patient_map;
        args->trackr=trackr;
        /* FOR GRADER - (line 300) The last argument in pthread_create used "&args". But args is already a pointer, so this should just be "args" */
        args->thread_id=pthread_create(&worker_threads[i],NULL,worker_thread_func,args);
        
    }
    
    for(int i=0; i<3; ++i){ //CREATING STATISTICS THREADS
        STFArgs *args = new STFArgs;
        args->n_reqs=nreq;
        args->patient_name=patient_names[i];
        args->PCB=patient_map[patient_names[i]];
        pthread_create(&statistics_threads[i],NULL,statistics_thread_func,args);
    }
    gettimeofday(&t2,NULL);
    std::cout << "Average time taken by server to make " << nreq << " requests:    " << (t2.tv_usec-t1.tv_usec)/nreq << " milliseconds/request" << std::endl;

    /*  FOR GRADER - lines 316-319 were useless, so I commented them out. 
        Sen(the TA) asked me where I got them and I don't remember. Possibly from some MP2 code*/
    // std::ofstream client_data;
    // std::ofstream server_data;
    // client_data.open("client_data.csv");
    // server_data.open("server_data.csv");
    
    /*  FOR GRADER - (lines 322-333) Here I implemented pthread_join for all Threads*/
    for(int i=0;i<3;++i){
        pthread_join(req_threads[i],NULL);
    }
    std::cout << "\nRequest Threads all done and closed\n" << std::endl; //DEBUG
    for(int i=0;i<nwt;++i){
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


