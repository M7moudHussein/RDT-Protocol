//
// Created by salma on 12/17/18.
//

#include <iostream>
#include "stop_and_wait_strategy.h"
#include "../packet_sender.h"

stop_and_wait_strategy::stop_and_wait_strategy(std::string file_name, int window_size, int max_window_size) {
    stop_and_wait_strategy::pkt_builder = new packet_builder(std::move(file_name), window_size);
    stop_and_wait_strategy::next_seq_number = 1;
    stop_and_wait_strategy::window_size = window_size;
    stop_and_wait_strategy::max_window_size = max_window_size;
    stop_and_wait_strategy::threshold = max_window_size;
    fill_window();
}

void stop_and_wait_strategy::start() {
    auto it = window.begin();
    while (it != window.end()) {
        stop_and_wait_strategy::send_packet(*it);
        if (it == window.begin()) {
            //initialize timer thread then sleep it till timeout of 1st unacked packet
            std::thread *th = new std::thread(&stop_and_wait_strategy::handle_time_out, this);
            timer = new timer_thread(th);
        }
        it++;
    }
}

void stop_and_wait_strategy::acknowledge_packet(ack_packet &ack_pkt) {
    auto it = window.begin();

    while (it != window.end()) {
        if (ack_pkt.get_ackno() == (*it)->get_seqno()) {
            // This check is to skip duplicate acks
            if (!(*it)->is_acked()) {
                std::cout << "Ack received for packet with seqno = " << (*it)->get_seqno() << std::endl;
                (*it)->set_ack(true);
                set_mutex.lock();
                unacked_packets.erase((*it));
                set_mutex.unlock();
                std::cout << "Removed packet with seqno = " << (*it)->get_seqno() << " from timer thread" << std::endl;
                if (it == window.begin()) {
                    stop_and_wait_strategy::advance_window();
                }
            }
            break;
        }
        it++;
    }
}

void stop_and_wait_strategy::send_packet(data_packet *packet) {
    std::cout << "Sending packet with seqno = " << packet->get_seqno() << std::endl;
    packet_sender::send_packet(server_socket, client_address, packet);
    packet->set_time_stamp(std::chrono::steady_clock::now());
    set_mutex.lock();
    unacked_packets.insert(packet);
    set_mutex.unlock();
}

void stop_and_wait_strategy::handle_time_out() {
// Timer thread work, re-sends packet whenever it times out.
    while (true) {
        if (!unacked_packets.empty()) {
            set_mutex.lock();
            data_packet *first_unacked_pkt = *(unacked_packets.begin());
            set_mutex.unlock();
            if (std::chrono::steady_clock::now() < first_unacked_pkt->get_time_stamp() + packet_util::PACKET_TIME_OUT) {
                std::cout << "Timer thread sleeping for 1 seconds..." << std::endl;
                timer->sleep_until(first_unacked_pkt->get_time_stamp() + packet_util::PACKET_TIME_OUT);
            } else {
                set_mutex.lock();

//                std::string unacked_packets_list;
//                for (auto pkt : unacked_packets) {
//                    unacked_packets_list += std::to_string(pkt->get_seqno()) + ", ";
//                }
//                unacked_packets_list.pop_back(), unacked_packets_list.pop_back();
//
//                std::cout << "Unacked packets: " << unacked_packets_list << std::endl;

                unacked_packets.erase(first_unacked_pkt);
                set_mutex.unlock();
                std::cout << "Timeout! Resending packet..." << std::endl;
                stop_and_wait_strategy::send_packet(first_unacked_pkt);
            }
        }
    }
}

void stop_and_wait_strategy::advance_window() {
    auto pkt_iter = window.begin();
    while (!window.empty()) {
        if ((*pkt_iter)->is_acked()) {
            packets_sent++;
            window.pop_front();
            std::cout << "Popped packet " << (*pkt_iter)->get_seqno() << std::endl;
            delete *pkt_iter;
            pkt_iter = window.begin();
            if (pkt_builder->has_next()) {
                std::cout << "pkt builder has next, sending next..." << std::endl;
                auto pkt = pkt_builder->get_next_packet(next_seq_number);
                window.push_back(pkt);
                stop_and_wait_strategy::send_packet(pkt);
            }
        } else {
            break;
        }
    }
}

