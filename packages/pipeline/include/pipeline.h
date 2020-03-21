#ifndef _PIPELINE_H_
#define _PIPELINE_H_

#include <config.h>
#include "stage.h"


class Pipeline {
private:
    Stage* input_stage;
    const char* sender_name;
    Tube* sender;
public:
    Pipeline(Stage* input_stage);
    ~Pipeline();
    void join();
    void put(cfg::data_t* item);
    void kill_pipeline();
};

#endif // _PIPELINE_H_