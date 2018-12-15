#include <utility>

//
// Created by mahmoud on 12/8/18.
//

#include "go_back_N_strategy.h"
#include "../../shared/packet_util.h"

#define DEFAULT_WINDOW_SIZE 200

go_back_N_strategy::go_back_N_strategy(std::string file_name) {
    window_size = DEFAULT_WINDOW_SIZE;
    pkt_builder = new packet_builder(std::move(file_name), window_size);
    fill_window();
}

void go_back_N_strategy::start() {
    for (auto it = window.begin(); it != window.end(); it++) {
        go_back_N_strategy::send_packet(*it);
        (*it)->set_time_stamp(std::chrono::steady_clock::now());

        if (it == window.begin()) {
            timer = new timer_thread(new std::thread(&go_back_N_strategy::handle_time_out, this));
        }
    }
}

void go_back_N_strategy::handle_time_out() {
    while (true) {
        set_mutex.lock();
        data_packet *first_unacked_pkt = *(unacked_packets.begin());
        set_mutex.unlock();
        if (first_unacked_pkt->get_time_stamp() + packet_util::PACKET_TIME_OUT < std::chrono::steady_clock::now())
            timer->sleep_until(first_unacked_pkt->get_time_stamp() + packet_util::PACKET_TIME_OUT);
        else {
            set_mutex.lock();
            std::vector<data_packet *> temp;
            for (auto it: unacked_packets) {
                it->set_time_stamp(std::chrono::steady_clock::now());
                go_back_N_strategy::send_packet(it);
                temp.push_back(it);
            }
            unacked_packets.clear();
            for (auto it: temp) {
                unacked_packets.insert(it);
            }
            set_mutex.unlock();
        }
    }
}

void go_back_N_strategy::acknowledge_packet(ack_packet &ack_pkt) {
    auto it = window.begin();
    while (it != window.end()) {
        if (ack_pkt.get_ackno() == (*it)->get_seqno()) {
            set_mutex.lock();
            auto back_it = it;
            while (back_it >= window.begin()) {
                (*back_it)->ack();
                unacked_packets.erase((*back_it));
                it--;
            }
            set_mutex.unlock();
            go_back_N_strategy::advance_window();
            break;
        }
    }
}

void go_back_N_strategy::send_packet(data_packet *packet) {
    sendto(server_socket, packet->pack().c_str(), packet->pack().length(),
           MSG_CONFIRM, (const struct sockaddr *) &client_address,
           sizeof client_address);
    set_mutex.lock();
    unacked_packets.insert(packet);
    set_mutex.unlock();
}

void go_back_N_strategy::advance_window() {
    //TODO implement this function
}

void go_back_N_strategy::expand_window() {
    //TODO implement this function
}

void go_back_N_strategy::shrink_window(int new_size) {
    //TODO implement this function
}

void go_back_N_strategy::adjust_window_size() {
    //TODO implement this function
}