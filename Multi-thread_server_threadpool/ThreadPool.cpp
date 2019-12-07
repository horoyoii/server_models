#pragma oncce

#include<pthread.h>
#include"EventQueue.h"

/**
* ThreaedPool
*
*/

typedef void (*dispatch_fn)(void*);
void* do_work(void*);

class ThreadPool{
private:
    pthread_t*              workers;
    int                     num_of_workers;
    EventQueue<task_t>      eventQueue;


public:
    ThreadPool(int num_of_workers)
        :num_of_workers(num_of_workers), eventQueue(){
        
    }
    
    void createWorkers(){
        workers = new pthread_t[num_of_workers];

        for(int i=0; i < num_of_workers; i++){
            if(pthread_create(&(workers[i]), NULL, do_work, &eventQueue) != 0){
                perror("pthread create : ");
                exit(1);
            }
        }
    }

    
    /**
    * This will be called by main thread when a new connection arrives. 
    * It sends a thread off to do some work.

    */
    void dispatch(dispatch_fn handler, void* arg){
        task_t newTask;
        newTask.routine = handler;
        newTask.arg = arg;
               
        // enqueue will wake up one thread if there is a sleeping one.
        eventQueue.enqueue(newTask);
    }

};

void* do_work(void* arg)
{
    EventQueue<task_t>* eq = (EventQueue<task_t>*)arg;
    task_t curTask;
    
    while(1)
    {
        curTask = eq->dequeue(); // dequeue will be blocked if there is no task to do. 
       
        // Do the job 
        (curTask.routine)(curTask.arg);

    }

}
