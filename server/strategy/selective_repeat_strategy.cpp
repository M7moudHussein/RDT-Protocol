#include <utility>
#include <iostream>

//
// Created by mahmoud on 12/7/18.
//

#include "selective_repeat_strategy.h"
#include "../../shared/packet_util.h"
#include "../packet_sender.h"

#define DEFAULT_WINDOW_SIZE 1

selective_repeat_strategy::selective_repeat_strategy(std::string file_name, int max_window_size) { // Selective Repeat
    selective_repeat_strategy::pkt_builder = new packet_builder(std::move(file_name), DEFAULT_WINDOW_SIZE);
    selective_repeat_strategy::window_size = DEFAULT_WINDOW_SIZE;
    selective_repeat_strategy::next_seq_number = 0;
    selective_repeat_strategy::max_window_size = max_window_size;
    selective_repeat_strategy::threshold = max_window_size / 2;
    fill_window();
}

selective_repeat_strategy::selective_repeat_strategy(std::string file_name, int window_size, int max_window_size) { // Stop-and-Wait
    selective_repeat_strategy::pkt_builder = new packet_builder(std::move(file_name), window_size);
    selective_repeat_strategy::window_size = window_size;
    selective_repeat_strategy::next_seq_number = 0;
    selective_repeat_strategy::max_window_size = max_window_size;
    selective_repeat_strategy::threshold = max_window_size;
    fill_window();
}

void selective_repeat_strategy::acknowledge_packet(ack_packet &ack_pkt) {
    auto it = window.begin();
    while (it != window.end()) {
        if (ack_pkt.get_ackno() == (*it)->get_seqno()) {
            // This check is to skip duplicate acks
            if (!(*it)->is_acked()) {
                std::cout << "Acknowledging packet with seqno = " << (*it)->get_seqno() << std::endl;
                (*it)->ack();
                set_mutex.lock();
                unacked_packets.erase((*it));
                set_mutex.unlock();
                std::cout << "Removed packet with seqno = " << (*it)->get_seqno() << " from timer thread" << std::endl;
                if (it == window.begin()) {
                    selective_repeat_strategy::advance_window();
                }
            }
            selective_repeat_strategy::adjust_window_size();
            selective_repeat_strategy::expand_window();
            break;
        }
        it++;
    }
}

void selective_repeat_strategy::expand_window() {
    while (window.size() < window_size && (!aux_window.empty() || pkt_builder->has_next())) {
        std::cout << "pkt builder has next, sending next..." << std::endl;
        auto pkt;
        if (!aux_window.empty())
            pkt = aux_window.pop_front();
        else
            pkt = pkt_builder->get_next_packet(next_seq_number);
        window.push_back(pkt);
        selective_repeat_strategy::send_packet(pkt);
    }
}

void selective_repeat_strategy::shrink_window(int new_size) {
    int new_threshold = window_size / 2;
    threshold = new_threshold < 1 ? 1 : new_threshold;
    window_size = new_size;

    auto it = window.begin() + window_size;
    while (it != window.end()) {
        aux_window.push_back(*it);
        unacked_packets.erase(*it);
        it = window.erase(it);
    }
}

void selective_repeat_strategy::adjust_window_size() {
    int new_size;
    if (window_size < threshold)
        new_size = window_size * 2;
    else
        new_size = window_size + 1;

    window_size = new_size > max_window_size ? max_window_size : new_size;
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
    // Timer thread work, re-sends packet whenever it times out.
    while (true) {
        if (!unacked_packets.empty()) {
            set_mutex.lock();
            data_packet *first_unacked_pkt = *(unacked_packets.begin());
            set_mutex.unlock();
            if (std::chrono::steady_clock::now() < first_unacked_pkt->get_time_stamp() + packet_util::PACKET_TIME_OUT) {
                std::cout << "Timer thread sleeping for 5 seconds..." << std::endl;
                timer->sleep_until(first_unacked_pkt->get_time_stamp() + packet_util::PACKET_TIME_OUT);
            } else {
                set_mutex.lock();
                unacked_packets.erase(first_unacked_pkt);
                set_mutex.unlock();
                std::cout << "Timeout! Resending packet..." << std::endl;
                shrink_window(1);
                selective_repeat_strategy::send_packet(first_unacked_pkt);
            }
        }
    }
}

void selective_repeat_strategy::send_packet(data_packet *packet) {
    std::cout << "Sending packet with seqno = " << packet->get_seqno() << std::endl;
    packet_sender::send_packet(server_socket, client_address, packet);
    packet->set_time_stamp(std::chrono::steady_clock::now());
    set_mutex.lock();
    unacked_packets.insert(packet);
    set_mutex.unlock();
}

void selective_repeat_strategy::advance_window() {
    auto pkt_iter = window.begin();
    while (!window.empty()) {
        if ((*pkt_iter)->is_acked()) {
            window.pop_front();
            std::cout << "Popped packet " << (*pkt_iter)->get_seqno() << std::endl;
            pkt_iter = window.begin();
        } else {
            break;
        }
    }
}
