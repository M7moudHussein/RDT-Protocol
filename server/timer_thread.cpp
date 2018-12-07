//
// Created by salma on 12/7/18.
//

#include "timer_thread.h"

timer_thread::timer_thread(std::thread *thread) {
    this->thread = thread;
}

void timer_thread::detach() {
    thread->detach();
}

//std::condition_variable timer_thread::get_cond_var() {
//    return this->cond_var;
//}
//
//std::mutex timer_thread::get_mutex() {
//    return this->mutex;
//}

timer_thread::~timer_thread() {
    delete timer_thread::thread;
}
