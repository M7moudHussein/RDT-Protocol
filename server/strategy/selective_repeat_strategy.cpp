#include <utility>

//
// Created by mahmoud on 12/7/18.
//

#include "selective_repeat_strategy.h"

#define DEFAULT_WINDOW_SIZE 10 //TODO just random window size... it should be changed to the right value

selective_repeat_strategy::selective_repeat_strategy(std::string file_name, int window_size) {
    selective_repeat_strategy::pkt_builder = new packet_builder(std::move(file_name), window_size);
    selective_repeat_strategy::next_seq_number = 0;
    selective_repeat_strategy::window_size = window_size;
    fill_window();
}

selective_repeat_strategy::selective_repeat_strategy(std::string file_name) {
    selective_repeat_strategy::pkt_builder = new packet_builder(std::move(file_name), window_size);
    selective_repeat_strategy::next_seq_number = 0;
    selective_repeat_strategy::window_size = DEFAULT_WINDOW_SIZE;
    fill_window();
}

void selective_repeat_strategy::acknowledge_packet(ack_packet &ack_pkt) {
    auto it = window.begin();
    while (it != window.end()) {
        if (ack_pkt.get_ackno() == (*it)->get_seqno()) {
            if (!(*it)->is_acked()) {//duplicate acks skip
                (*it)->ack();
                set_mutex.lock();
                unacked_packets.erase((*it));
                set_mutex.unlock();
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
            std::thread *th = new std::thread(&selective_repeat_strategy::handle_time_out, this);
            timer = new timer_thread(th);
        }
        it++;
    }

}

void selective_repeat_strategy::handle_time_out() {
    // Timer thread work, resends packet whenever it times out.
    while (true) {
        set_mutex.lock();
        data_packet *first_unacked_pkt = *(unacked_packets.begin());
        set_mutex.unlock();
        if (first_unacked_pkt->get_time_stamp() + PACKET_TIME_OUT < std::chrono::steady_clock::now())
            timer->sleep_until(first_unacked_pkt->get_time_stamp() + PACKET_TIME_OUT);
        else {
            set_mutex.lock();
            unacked_packets.erase(first_unacked_pkt);
            set_mutex.unlock();
            selective_repeat_strategy::send_packet(first_unacked_pkt);
        }
    }
}

void selective_repeat_strategy::send_packet(data_packet *packet) {
    sendto(server_socket, packet->pack().c_str(), packet->pack().length(),
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
            window.push_back(pkt_builder->get_next_packet(next_seq_number));
                selective_repeat_strategy::send_packet(*it);
    }
}