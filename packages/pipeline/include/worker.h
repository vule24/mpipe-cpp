#ifndef _WORKER_H_
#define _WORKER_H_

#include <iostream>
#include <vector>
#include <unistd.h>
#include <config.h>
#include "tube.h"
class Stage;
#include "stage.h"

class Worker {
protected:
    int id;
    std::string receiver_name;
    std::vector<std::string> sender_names;
    Tube* receiver;
    std::vector<Tube*> senders;
    cfg::data_t* item;
    bool is_origin;
    virtual void do_init() = 0;
    virtual void receive_task();
    virtual int do_task() = 0;
    virtual void put_result();
    virtual Worker* clone() = 0;
public:
    static const int CODE_PUT_RESULT = 0;
    static const int CODE_STOP = 1;
    static const int CODE_SKIP_RESULT = 2;
    Worker(int id);
    virtual ~Worker();
    void set_id(int new_id);
    int get_id();
    void set_receiver_name(std::string receiver_name);
    void add_sender(std::string sender_name);
    void init_tubes();
    Worker* get_clone();
    void run(Stage* stage, int* pipefd);
};

// Child class: child worker clone {return new <child>(*this)};


#endif // _WORKER_H_