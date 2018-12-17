#include <utility>
#include <iostream>

//
// Created by mahmoud on 12/8/18.
//

#include "go_back_N_strategy.h"
#include "../../shared/packet_util.h"
#include "../packet_sender.h"

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
        if (std::chrono::steady_clock::now() < first_unacked_pkt->get_time_stamp() + packet_util::PACKET_TIME_OUT) {
            std::cout << "Timer thread sleeping for 1 seconds..." << std::endl;
            timer->sleep_until(first_unacked_pkt->get_time_stamp() + packet_util::PACKET_TIME_OUT);
        } else {
            set_mutex.lock();
            std::vector<data_packet *> temp;
            for (auto it: unacked_packets) {
                it->set_time_stamp(std::chrono::steady_clock::now());
                std::cout << "Timeout!Resending pakcet....." << std::endl;
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
            std::cout << "Ack received for packet with seqno = " << (*it)->get_seqno() << std::endl;
            set_mutex.lock();
            auto back_it = it;
            while (back_it >= window.begin()) {
                (*back_it)->set_ack(true);
                unacked_packets.erase((*back_it));
                std::cout << "Removed packet with seqno = " << (*back_it)->get_seqno() << " from timer thread"
                          << std::endl;
                back_it--;
            }
            set_mutex.unlock();
            go_back_N_strategy::advance_window();
            break;
        }
        it++;
    }
}

void go_back_N_strategy::send_packet(data_packet *packet) {
    std::cout << "Sending packet with seqno = " << packet->get_seqno() << std::endl;
    packet_sender::send_packet(server_socket, client_address, packet);
    packet->set_time_stamp(std::chrono::steady_clock::now());
    set_mutex.lock();
    unacked_packets.insert(packet);
    set_mutex.unlock();
}

void go_back_N_strategy::advance_window() {
    auto pkt_iter = window.begin();
    while (!window.empty()) {
        if ((*pkt_iter)->is_acked()) {
            window.pop_front();
            std::cout << "Popped packet " << (*pkt_iter)->get_seqno() << std::endl;
            pkt_iter = window.begin();
            if (pkt_builder->has_next()) {
                std::cout << "pkt builder has next, sending next..." << std::endl;
                auto pkt = pkt_builder->get_next_packet(next_seq_number);
                window.push_back(pkt);
                go_back_N_strategy::send_packet(pkt);
            }
        } else {
            break;
        }
    }
}
