#include "tube.h"

BoostSharedMemTube::BoostSharedMemTube(const char* name, const char role){
    try{
        this->role = role;
        this->name = name;
        if (this->role == BoostSharedMemTube::FIRST_SENDER) {
            shared_memory_object::remove(this->name);
            shared_memory_object shm(
                open_or_create,
                this->name,
                read_write
            );
            shm.truncate(sizeof(SharedMemoryBuffer));
            this->region = mapped_region(shm, read_write);
            void* addr = this->region.get_address();
            this->data = new (addr) SharedMemoryBuffer;
        }
        else if (this->role == BoostSharedMemTube::SENDER) {
            shared_memory_object shm(
                open_or_create,
                this->name,
                read_write
            );
            shm.truncate(sizeof(SharedMemoryBuffer));
            this->region = mapped_region(shm, read_write);
            void* addr = this->region.get_address();
            this->data = new (addr) SharedMemoryBuffer;
        }

        if (this->role == BoostSharedMemTube::RECEIVER) {
            shared_memory_object shm(
                open_or_create,
                this->name,
                read_write
            );
            this->region = mapped_region(shm, read_write);
            void* addr = this->region.get_address();
            this->data = static_cast<SharedMemoryBuffer*>(addr);
        }
    }
    catch(interprocess_exception &e) {
        shared_memory_object::remove(this->name);
        std::cout << "[Boost] Failed to init: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

BoostSharedMemTube::~BoostSharedMemTube(){
    shared_memory_object::remove(this->name);
}

void BoostSharedMemTube::send(cfg::data_t* item) {
    // std::cout << this->name << " sending:" << item << std::endl;
    try{
        this->data->nempty.wait();
        this->data->mutex.wait();
        int send_pivot = this->data->send_pivot;
        this->data->items[send_pivot] = *item;
        this->data->send_pivot = (send_pivot + 1) % SharedMemoryBuffer::NumItems;
        this->data->mutex.post();
        this->data->nstored.post();
    }
    catch (interprocess_exception &e) {
        shared_memory_object::remove(this->name);
        std::cout << "[Boost] Failed to send: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

cfg::data_t* BoostSharedMemTube::receive() {
    try{
        cfg::data_t* extracted_item;
        this->data->nstored.wait();
        this->data->mutex.wait();
        int receive_pivot = this->data->receive_pivot;
        extracted_item = new cfg::data_t(this->data->items[receive_pivot]);
        this->data->receive_pivot = (receive_pivot + 1) % SharedMemoryBuffer::NumItems;
        this->data->mutex.post();
        this->data->nempty.post();
        return extracted_item;
    }
    catch (interprocess_exception &e) {
        shared_memory_object::remove(this->name);
        std::cout << "[Boost] Failed to receive: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

