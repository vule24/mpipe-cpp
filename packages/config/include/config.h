#ifndef _CONFIG_H_
#define _CONFIG_H_
#include <iostream>
// define tube protocol class name
#define TUBE BoostSharedMemTube
#define NUM_ITEMS_BOOST_SHARED_MEM 50
#define FPS 30

// decide type of element in pipeline buffer
namespace pds {
    struct DataFlow {
        float x;
        float y;
    };
}

namespace cfg {
    typedef pds::DataFlow data_t;
};

#endif // _CONFIG_H_