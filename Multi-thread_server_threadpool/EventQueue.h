#include<queue>
#include<mutex>
#include<condition_variable>

using namespace std;


typedef struct Task{
    void    (*routine)(void*);
    void*   arg;
   
}task_t;


template<typename T>
class EventQueue{
private:
    queue<T>            eq;
    mutex               m;
    condition_variable  c;    

public:
    EventQueue()
        :eq(), m(), c(){
        
    }

    void enqueue(T t){
        lock_guard<mutex> lock(m); 
        // lock_guard will be locked only construction and unlocked on destruction
                
        eq.push(t);
        c.notify_one();
    }


    T dequeue(void){
        unique_lock<mutex> lock(m);
        /*
            lock_guard and unique_lock both are for RAII pattern.
            but unique_lock can be locked and unlocked anytime. 
        */

        while(eq.empty()){
            printf("wait ---\n");
            c.wait(lock);   // release mutex lock   
        }
        
        printf("wakeup\n");
        T res = eq.front();
        eq.pop();
        
     
        return res;
    }


};
