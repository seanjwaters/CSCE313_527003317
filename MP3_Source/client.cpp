/* 
    File: simpleclient.cpp

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2019/09/23

    Simple client main program for MP2 in CSCE 313
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
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#include <fstream>
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

void * request_thread_func(int n_reqs, string patient_name, PCBuffer PCB ){
    for(int i=0; i<n_reqs; ++i){
        string req = "dat" + patient_name;
        PCB.Deposit(req);
    }
}





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

    //mutexs and threads
    Mutex m;
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
    PCBuffer PCB(size);
    pthread_t *req_threads = new pthread_t[3]; 
    pthread_t *worker_threads = new pthread_t[3];
    pthread_t *stat_threads = new pthread_t[3];
    
    for(){
        pthread_create(&a,NULL,request_thread_func,);
    }
    for(){
        pthread_create(&a,NULL,worker_thread_func,);
    }
    for(){
        pthread_create(&a,NULL,statistics_thread_func,);
    }
        

    
    std::ofstream client_data;
    std::ofstream server_data;
    client_data.open("client_data.csv");
    server_data.open("server_data.csv");
    


    std::cout << "Establishing control channel... " << std::flush;
    RequestChannel chan("control", RequestChannel::Side::CLIENT);
    std::cout << "done." << std::endl;
    
    /* -- Start sending a sequence of requests */
    // {
    // MutexGuard mg(m);
    
    // gettimeofday(&t1,NULL);
    // std::string reply1 = chan.send_request("hello");
    // gettimeofday(&t2,NULL);
    // std::cout << "Reply to request 'hello' is '" << reply1 << "'" << std::endl;
    // std::cout << "Time taken to make the request is: " << t2.tv_usec-t1.tv_usec << std::endl;
    // }

    // gettimeofday(&t1,NULL);
    // std::string reply2 = chan.send_request("data Joe Smith");
    // gettimeofday(&t2,NULL);
    // std::cout << "Reply to request 'data Joe Smith' is '" << reply2 << "'" << std::endl;
    // std::cout << "Time taken to make the request is: " << t2.tv_usec-t1.tv_usec << std::endl;

    // gettimeofday(&t1,NULL);
    // std::string reply3 = chan.send_request("data Jane Smith");
    // gettimeofday(&t2,NULL);
    // std::cout << "Reply to request 'data Jane Smith' is '" << reply3 << "'" << std::endl;
    // std::cout << "Time taken to make the request is: " << t2.tv_usec-t1.tv_usec << std::endl;
    

    // GENERATING DATA FROM SERVER

    {
        MutexGuard mg(m);

        gettimeofday(&t1,NULL);
        for(int i = 0; i < 100; i++) {
            std::string request_string("data TestPerson" + int2string(i));

            gettimeofday(&n1,NULL);
            std::string reply_string = chan.send_request(request_string);
            gettimeofday(&n2,NULL);
            server_data << i << "," << (n2.tv_usec-n1.tv_usec) << std::endl;

            std::cout << "reply to request " << i << ":" << reply_string << std::endl;;
        }
        gettimeofday(&t2,NULL);
        std::cout << "Average time taken by server to make 100 requests:    " << (t2.tv_usec-t1.tv_usec)/100 << " milliseconds/request" << std::endl;
    }

    // GENERATING DATA FROM CLIENT

    {
        MutexGuard mg(m);

        gettimeofday(&t1,NULL);
        for(int i = 0; i < 100; i++) {
            std::string request_string("data TestPerson" + int2string(i));

            gettimeofday(&n1,NULL);
            std::string reply_string = generate_data();
            gettimeofday(&n2,NULL);
            client_data << i << "," << (n2.tv_usec-n1.tv_usec) << std::endl;

            std::cout << "reply to request " << i << ":" << reply_string << std::endl;;
        }
        gettimeofday(&t2,NULL);
        std::cout << "Average time taken by client to make 100 requests:    " << (t2.tv_usec-t1.tv_usec)/100 << " milliseconds/request" << std::endl;
    }
    


    std::string reply4 = chan.send_request("quit");
    std::cout << "Reply to request 'quit' is '" << reply4 << "'" << std::endl;



    usleep(1000000);
}


