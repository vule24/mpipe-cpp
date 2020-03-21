#define MSGSIZE 10
#include "stage.h"

Stage::Stage(Worker* input_worker, int num_workers, std::string sender_name) {
    this->num_sender = 0;
    this->sender_name = sender_name;
    this->workers.push_back(input_worker);
    this->num_workers = num_workers;
    for (int i = 1; i < this->num_workers; i++) {
        Worker* clone_worker = input_worker->get_clone();
        clone_worker->set_id(input_worker->get_id() + i);
        this->workers.push_back(clone_worker);
    }
}

Stage::~Stage() {
    for (auto&& stage : this->next_stages) {
        delete stage;
        stage = nullptr;
    }
    for (auto&& worker : this->workers) {
        delete worker;
        worker = nullptr;
    }
};

void Stage::set_receiver_name(std::string receiver_name) {
    for (auto worker : this->workers) {
        worker->set_receiver_name(receiver_name);
    }
}

void Stage::set_sender_name(std::string sender_name) {
    for (auto worker : this->workers) {
        worker->add_sender(sender_name);
    }
}

// link to the next stage. Make sure sender_name is parsed at Constructor
void Stage::link(Stage* next_stage) {
    assert(!this->sender_name.empty());
    std::string tube_name = this->sender_name + std::to_string(this->num_sender);
    // cout << tube_name << endl;
    this->set_sender_name(tube_name);
    next_stage->set_receiver_name(tube_name);
    this->next_stages.push_back(next_stage);
    this->num_sender++;
}

// link to a tube using tube_name. Make sure sender_name is empty
void Stage::link(std::string tube_name) {
    assert(this->sender_name.empty());
    this->set_sender_name(tube_name);
    this->num_sender++;
}

// fork proccesses for all workers
// in current stage and all next stage
void Stage::build() {
    for (auto worker : this->workers) {
        //create num_workers pipe before fork new proccess
        // int* pipefd = new int[2];
        int pipefd[2];
        if (pipe(pipefd) < 0) {
            std::cout << "Failed to create pipe\n";
            exit(EXIT_FAILURE);
        }
        // this->pipefd.push_back(pipefd);
        
        // start a child proccess
        pid_t pid = fork();
        if (pid < 0) {
            // cannot create proccess
            std::cerr << "Cannot create proccess\n";
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) {
            //this is child process for worker
            //if worker run out of infinite loop 
            //then terminate the proccess
            
            //init tubes for worker
            worker->init_tubes();

            // worker start to run and claim done when finish do_init
            worker->run(this, pipefd);
            std::cout << "Exit proccess: " << getpid() << std::endl;
            exit(EXIT_SUCCESS);
        }
        //this is parent proccess
        //keep iterating over all workers
        this->workers_pids.push_back(pid);
        //wait for workers claiming done
        this->wait_for_workers_initialize(pipefd);
    }
    // build downstream recursively
    for (auto next_stage : this->next_stages) {
        next_stage->build();
    }
}

// wait for all forked proccesses to exit
void Stage::join() {
    for (auto next_stage : this->next_stages){
        next_stage->join();
    }
    for (int i = 0; i < this->num_workers; i++) {
        wait(NULL);
    }
}

// get all pids that are available in the downstream
// only use at the first stage
// pipeline call this to kill all proccesses with pids
std::vector<pid_t> Stage::get_all_pids() {
    if (this->next_stages.empty()) {
        return this->workers_pids;
    }
    else {
        std::vector<pid_t> forward_pids = this->workers_pids;
        for (auto next_stage : this->next_stages) {
            std::vector<pid_t> next_stage_pids = next_stage->get_all_pids();
            std::vector<pid_t> temp_pids;
            temp_pids.reserve(forward_pids.size() + next_stage_pids.size());
            temp_pids.insert(temp_pids.end(), forward_pids.begin(), forward_pids.end());
            temp_pids.insert(temp_pids.end(), next_stage_pids.begin(), next_stage_pids.end());
            forward_pids = temp_pids;
        }
        return forward_pids;
    }
}

// each worker will send "done" message 
//to the parent proccess
void Stage::worker_claim_done(int* pipefd) {
    write(pipefd[1], "done", MSGSIZE);
    close(pipefd[1]);
    std::string sn = this->sender_name.empty() ? "last_stage" : this->sender_name;
    // cout << sn << "-send:\tdone\n";
}

// wait for "done" message to be sent 
// to parent proccess in this stage 
void Stage::wait_for_workers_initialize(int* pipefd){
    char inbuf[MSGSIZE];
    int n_bytes;
    close(pipefd[1]);
    while((n_bytes = read(pipefd[0], inbuf, MSGSIZE)) > 0){
        std::string sn = this->sender_name.empty() ? "last_stage" : this->sender_name;
        // cout << sn << "-receive:\t" << inbuf << endl;
        if (strcmp(inbuf,"done") == 0) break;
    }
}