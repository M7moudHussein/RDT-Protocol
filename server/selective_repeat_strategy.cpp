#include <utility>

//
// Created by mahmoud on 12/7/18.
//

#include "selective_repeat_strategy.h"

#define DEFAULT_WINDOW_SIZE 200 //TODO just random window size... it should be changed to the right value

selective_repeat_strategy::selective_repeat_strategy(std::string file_name, int window_size) {
    this->pkt_builder = new packet_builder(std::move(file_name), window_size);
}

selective_repeat_strategy::selective_repeat_strategy(std::string file_name) {
    window_size = DEFAULT_WINDOW_SIZE;
    this->pkt_builder = new packet_builder(std::move(file_name), window_size);
    //fill queue of unacked packets with N packets initially
    fill_window();
}

void selective_repeat_strategy::acknowledge_packet(ack_packet ack_pkt) {
    auto it = window.begin();
    while (it != window.end()) {
        if (ack_pkt.get_ackno() == (*it)->get_seqno()) {
            if (!(*it)->is_acked()) {//duplicate acks skip
                (*it)->ack();
                if (it == window.begin())
                    selective_repeat_strategy::advance_window();
            }
            break;
        }
    }
}

void selective_repeat_strategy::start() {
    auto it = window.begin();
    while (it != window.end()) {
        selective_repeat_strategy::send_packet(*it);
        if (it == window.begin()) {
            //initialize timer thread then sleep it till timeout of 1st unacked packet
            timer = new timer_thread(new std::thread(&selective_repeat_strategy::handle_time_out));
        }
        it++;
    }

}

void selective_repeat_strategy::fill_window() {
    for (int i = 0; i < window_size && pkt_builder->has_next(); i++) {
        window.push_back(pkt_builder->get_next_packet());
    }
}

void selective_repeat_strategy::send_packet(data_packet *packet) {
    std::stringstream pkt_ss;
    pkt_ss << packet;
    sendto(server_socket, pkt_ss.str().c_str(), pkt_ss.str().length(),
           MSG_CONFIRM, (const struct sockaddr *) &client_address,
           sizeof client_address);
    packet->set_time_stamp(std::chrono::steady_clock::now());
    set_mutex.lock();
    unacked_packets.insert(packet);
    set_mutex.unlock();
}

void selective_repeat_strategy::advance_window() {
    for (auto it = window.begin(); it != window.end() && (*it)->is_acked(); it++) {
        window.pop_front();
        if (pkt_builder->has_next())
            window.push_back(pkt_builder->get_next_packet());
    }
}