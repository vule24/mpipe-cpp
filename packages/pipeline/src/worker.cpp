#include "worker.h"

Worker::Worker(int id) {
    this->id = id;
    this->receiver_name = "";
    this->receiver = nullptr;
    this->is_origin = true;
    this->item = nullptr;
}

Worker::~Worker() {
    delete this->receiver;
    this->receiver = nullptr;
    for (auto&& sender : this->senders) {
        delete sender;
        sender = nullptr;
    }
    delete this->item;
    this->item = nullptr;
}

void Worker::set_id(int new_id) {
    this->id = new_id;
}

int Worker::get_id() {
    return this->id;
}

void Worker::set_receiver_name(std::string receiver_name) {
    if (!receiver_name.empty()) {
        this->receiver_name = receiver_name;
    }
}

void Worker::add_sender(std::string sender_name) {
    if (!sender_name.empty())
        this->sender_names.push_back(sender_name);
}   

// init tubes for sender and receiver 
void Worker::init_tubes() {
    if (!this->receiver_name.empty())
        this->receiver = new Tube(this->receiver_name.c_str(), Tube::RECEIVER);

    char mode = this->is_origin ? Tube::FIRST_SENDER : Tube::SENDER;
    for (auto sender_name : this->sender_names) {
        this->senders.push_back(new Tube(sender_name.c_str(), mode));
        // std::cout << "Init:" << sender_name << std::endl;
    };
}

void Worker::receive_task() {
    if (this->receiver != nullptr) {
        delete this->item;
        this->item = this->receiver->receive(); 
    }
}

// override this function if needed
// default is send item to the next stage
void Worker::put_result() {
    for (auto sender : this->senders) {
        sender->send(this->item);
    }
}

// set worker to be not origin then return cloned worker
Worker* Worker::get_clone() {
    Worker* cloned_worker = this->clone();
    cloned_worker->is_origin = false;
    return cloned_worker;
}


void Worker::run(Stage* stage, int* pipefd) {
    // cout << getpid() << ": start to run\n"; 
    this->do_init();
    stage->worker_claim_done(pipefd);
    while(true) {
        this->receive_task();        
        int code = this->do_task();
        switch (code) {
            case Worker::CODE_STOP: return;
            case Worker::CODE_SKIP_RESULT: continue;
            case Worker::CODE_PUT_RESULT:
                this->put_result();
        }
    }
}