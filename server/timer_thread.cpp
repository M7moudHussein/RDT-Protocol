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

timer_thread::~timer_thread() {
    delete timer_thread::thread;
}

void timer_thread::sleep_until(std::chrono::steady_clock::time_point time) {
    std::this_thread::sleep_until(time);
}
