#ifndef _WORKER_A_H_
#define _WORKER_A_H_

#include <iostream>
#include <config.h>
#include <worker.h>


class WorkerA : public Worker {
private:
    float sum;
protected:
    WorkerA* clone() {
        //just modify the class name for this function (obligatory)
        return new WorkerA(*this);
    }

    void do_init(){
        //this would run once in child process
        std::cout << "Worker A init\n";
    };

    int do_task(){
        //this would do the loop and return the Worker Code
        // CODE_STOP | CODE_SKIP_RESULT | CODE_PUT_RESULT
        this->sum = this->item->x + this->item->y;
        std::cout << "WorkerA report sum: " << this->sum << std::endl;
        return this->CODE_PUT_RESULT;
    };

public:
    WorkerA(int id) : Worker(id){
        // set some variables (in main process)
    };

    ~WorkerA(){
        // remember to delete your class pointer variable or something
    };
};

#endif // _WORKER_A_H_