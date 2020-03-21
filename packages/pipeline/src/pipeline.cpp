#include <pipeline.h>

Pipeline::Pipeline(Stage* input_stage) {
    this->sender_name = "head";
    this->sender = new Tube(this->sender_name, Tube::FIRST_SENDER);
    this->input_stage = input_stage;
    this->input_stage->set_receiver_name(this->sender_name);
    this->input_stage->build();
}

Pipeline::~Pipeline() {
    delete this->input_stage;
    this->input_stage = nullptr;
    delete this->sender;
    this->input_stage = nullptr;
};

// wait for all proccess terminate
void Pipeline::join() {
    this->input_stage->join();
}

void Pipeline::put(cfg::data_t* item) {
    this->sender->send(item);
}

void Pipeline::kill_pipeline() {
    std::vector<pid_t> pids = this->input_stage->get_all_pids();
    for (auto pid : pids) {
        std::cout << "Kill: " << pid << std::endl;
        kill(pid, SIGINT);
    }
    this->join();
}
