#ifndef _STAGE_H_
#define _STAGE_H_

#include <vector>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <config.h>
class Worker;
#include "worker.h"

class Stage {
private:
    std::string stage_name;
    int num_workers;
    std::vector<Worker*> workers;
    std::string sender_name;
    int num_sender;
    std::vector<Stage*> next_stages;
    std::vector<pid_t> workers_pids;
    void wait_for_workers_initialize(int* pipefd);
public:
    Stage(Worker* worker, int num_workers=1, std::string sender_name="");
    ~Stage();
    void set_receiver_name(std::string receiver_name);
    void set_sender_name(std::string sender_name);
    void link(Stage* next_stage);
    void link(std::string tube_name);
    void build();
    void join();
    std::vector<pid_t> get_all_pids();
    void worker_claim_done(int* pipefd);
};

#endif // _STAGE_H_