//
// Created by awalid on 12/7/18.
//

#ifndef RDT_PROTOCOL_WORKER_THREAD_H
#define RDT_PROTOCOL_WORKER_THREAD_H


#include <thread>

class worker_thread {
public:
    worker_thread(std::thread *th);

    void detach();
    void kill();
    void mark_done();
    bool is_done();
    std::thread::id get_thread_id();
    virtual ~worker_thread();

private:
    std::thread *thread;
    bool done;
};


#endif //RDT_PROTOCOL_WORKER_THREAD_H
