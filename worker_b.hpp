#ifndef _WORKER_B_H_
#define _WORKER_B_H_

#include <iostream>
#include <config.h>
#include <worker.h>


class WorkerB : public Worker {
private:
protected:
    WorkerB* clone() {
        //just modify the class name for this function (obligatory)
        return new WorkerB(*this);
    }

    void do_init(){
        //this would run once in child process
        std::cout << "Worker B init\n";
    };

    int do_task(){
        //this would do the loop in child process and return the Worker Code
        // CODE_STOP | CODE_SKIP_RESULT | CODE_PUT_RESULT
        std::cout << "Worker B display receive item: (" 
                << this->item->x 
                << ", " 
                << this->item->y 
                << ")\n";
        return this->CODE_SKIP_RESULT;
    };

public:
    WorkerB(int id) : Worker(id){
        // set some variables (in main process)
    };

    ~WorkerB(){
        // remember to delete your class pointer variable or something
    };
};

#endif // _WORKER_B_H_