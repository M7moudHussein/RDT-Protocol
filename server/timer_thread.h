//
// Created by salma on 12/7/18.
//

#ifndef RDT_PROTOCOL_TIMER_THREAD_H
#define RDT_PROTOCOL_TIMER_THREAD_H


#include "../shared/data_packet.h"
#include <set>
#include <thread>
#include <mutex>
#include <condition_variable>

class timer_thread {
public:
    timer_thread(std::thread *thread);
    void detach();
    std::mutex get_mutex();
    std::condition_variable get_cond_var();
    virtual ~timer_thread();

private:
    std::thread *thread;
    std::mutex mutex;
    std::condition_variable cond_var;

};


#endif //RDT_PROTOCOL_TIMER_THREAD_H
