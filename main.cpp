#include <pipeline.h>
#include "worker_a.hpp"
#include "worker_b.hpp"

using namespace std;
using namespace pds;

int main(int argc, char* argv[]) {
    Worker* worker_a = new WorkerA(1);
    Worker* worker_b = new WorkerB(2);

    Stage* stage_a = new Stage(worker_a, 2, "stage_a");
    Stage* stage_b = new Stage(worker_b, 1);

    stage_a->link(stage_b);

    Pipeline* pipeline = new Pipeline(stage_a);

    pds::DataFlow input_item;
    for (int i = 0; i < 10; i++) {
        input_item.x = (float)(2*i);
        input_item.y = (float)(3*i);
        pipeline->put(&input_item);
    }

    sleep(5);
    pipeline->kill_pipeline();
    return 0;
}