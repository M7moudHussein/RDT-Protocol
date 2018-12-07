//
// Created by awalid on 12/7/18.
//

#include "worker_thread.h"

worker_thread::worker_thread(std::thread *th) {
    worker_thread::thread = th;
    worker_thread::done = false;
}

std::thread::id worker_thread::get_thread_id() {
    return worker_thread::thread->get_id();
}

void worker_thread::detach() {
    worker_thread::thread->detach();
}

void worker_thread::kill() {
    pthread_cancel(worker_thread::thread->native_handle());
}

void worker_thread::mark_done() {
    worker_thread::done = true;
}

bool worker_thread::is_done() {
    return worker_thread::done;
}

worker_thread::~worker_thread() {
    delete worker_thread::thread;
}
