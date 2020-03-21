#ifndef _TUBE_H_
#define _TUBE_H_

#include <iostream>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <config.h>

using namespace boost::interprocess;

class BaseTube;
class BoostSharedMemTube;
class GrpcTube;

#ifndef TUBE
#define TUBE BoostSharedMemTube
#endif

typedef TUBE Tube;

class BaseTube {
protected:
    const char* name;
    char role;
public:
    static const char FIRST_SENDER = 0;
    static const char SENDER = 1;
    static const char RECEIVER = 2;
    BaseTube(){};
    BaseTube(const char* name, const char role);
    ~BaseTube() {};
    virtual void send(cfg::data_t* item) = 0;
    virtual cfg::data_t* receive() = 0;
};


#ifndef NUM_ITEMS_BOOST_SHARED_MEM
#define NUM_ITEMS_BOOST_SHARED_MEM 10
#endif

struct SharedMemoryBuffer {
        enum { NumItems = NUM_ITEMS_BOOST_SHARED_MEM };

        SharedMemoryBuffer(): 
        mutex(1), 
        nempty(NumItems), 
        nstored(0), 
        send_pivot(0),
        receive_pivot(0)
        {}

        // Semaphores to protect and synchronize access
        boost::interprocess::interprocess_semaphore mutex, nempty, nstored;

        // Items to fill
        cfg::data_t items[NumItems];
        int send_pivot;
        int receive_pivot;
    };

class BoostSharedMemTube : public BaseTube {
    private:
        mapped_region region;
        SharedMemoryBuffer* data;
    public:
        BoostSharedMemTube(const char* name, const char role);
        ~BoostSharedMemTube();
        void send(cfg::data_t* item);
        cfg::data_t* receive();
};

#endif // _TUBE_H_