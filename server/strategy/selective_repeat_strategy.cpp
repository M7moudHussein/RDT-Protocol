#include <utility>
#include <iostream>

//
// Created by mahmoud on 12/7/18.
//

#include "selective_repeat_strategy.h"
#include "../../shared/packet_util.h"
#include "../packet_sender.h"

#define DEFAULT_WINDOW_SIZE 1 //TODO just random window size... it should be changed to the right value

selective_repeat_strategy::selective_repeat_strategy(std::string file_name, int max_window_size) { // Selective Repeat
    selective_repeat_strategy::pkt_builder = new packet_builder(std::move(file_name), DEFAULT_WINDOW_SIZE);
    selective_repeat_strategy::next_seq_number = 1;
    selective_repeat_strategy::window_size = DEFAULT_WINDOW_SIZE;
    selective_repeat_strategy::max_window_size = max_window_size;
    selective_repeat_strategy::threshold = max_window_size / 2;
    fill_window();
}

void selective_repeat_strategy::acknowledge_packet(ack_packet &ack_pkt) {
    if (ack_pkt.get_cksum() == packet_util::calculate_checksum(&ack_pkt)) {
        wnd_mutex.lock();
        auto it = window.begin();
        while (it != window.end()) {
            if (ack_pkt.get_ackno() == (*it)->get_seqno()) {
                if (!(*it)->is_acked()) { // skip duplicate ACKs
                    std::cout << "Ack received for packet with seqno = " << (*it)->get_seqno() << std::endl;
                    (*it)->set_ack(true);
                    set_mutex.lock();
                    unacked_packets.erase((*it));
                    set_mutex.unlock();
                    std::cout << "Removed packet with seqno = " << (*it)->get_seqno() << " from timer thread" << std::endl;
                    if (it == window.begin())
                        selective_repeat_strategy::advance_window();

                    selective_repeat_strategy::adjust_window_size();
                    selective_repeat_strategy::expand_window();
                }
                break;
            }
            it++;
        }
        wnd_mutex.unlock();
    }
}

void selective_repeat_strategy::expand_window() {
    while (window.size() < window_size && (!aux_window.empty() || pkt_builder->has_next())) {
        data_packet *pkt;
        if (!aux_window.empty()) {
            pkt = *(aux_window.begin());
            aux_window.erase(aux_window.begin());
        } else {
            pkt = pkt_builder->get_next_packet(next_seq_number);
        }
        pkt->set_ack(false);
        window.push_back(pkt);
        selective_repeat_strategy::send_packet(pkt);
    }
}

void selective_repeat_strategy::shrink_window(int new_size) {
    std::cout << "SHRINKING................." << std::endl;
    int new_threshold = window_size / 2;
    threshold = new_threshold < 1 ? 1 : new_threshold;
    window_size = new_size;

    auto it = window.begin() + window_size;
    while (it != window.end()) {
        aux_window.insert(*it);
        set_mutex.lock();
        unacked_packets.erase(*it);
        set_mutex.unlock();
        it = window.erase(it);
    }
//    std::cout << "Aux window: ";
//    for (auto p : aux_window) {
//        std::cout << p->get_seqno() << " ";
//    }
    std::cout << std::endl;
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
    wnd_mutex.lock();
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
    wnd_mutex.unlock();
}

void selective_repeat_strategy::handle_time_out() {
    // Timer thread work, re-sends packet whenever it times out.
    while (true) {
        if (!unacked_packets.empty()) {
            set_mutex.lock();
            data_packet *first_unacked_pkt = *(unacked_packets.begin());
            set_mutex.unlock();
            if (std::chrono::steady_clock::now() < first_unacked_pkt->get_time_stamp() + packet_util::PACKET_TIME_OUT) {
//                std::cout << "Timer thread sleeping for 5 seconds..." << std::endl;
                timer->sleep_until(first_unacked_pkt->get_time_stamp() + packet_util::PACKET_TIME_OUT);
            } else {
                set_mutex.lock();
                std::string unacked_packets_list;
                for (auto pkt : unacked_packets) {
                    unacked_packets_list += std::to_string(pkt->get_seqno()) + ", ";
                }
                unacked_packets_list.pop_back(), unacked_packets_list.pop_back();

                std::cout << "Unacked packets: " << unacked_packets_list << std::endl;

                unacked_packets.erase(first_unacked_pkt);
                set_mutex.unlock();

                wnd_mutex.lock();
                std::string window_packets_list;
                for (auto pkt : window) {
                    window_packets_list += std::to_string(pkt->get_seqno()) + ", ";
                }
                window_packets_list.pop_back(), window_packets_list.pop_back();

                std::cout << "Window packets: " << window_packets_list << std::endl;
                wnd_mutex.unlock();

                std::cout << "Timeout! Resending packet..." << std::endl;
                wnd_mutex.lock();
                shrink_window(1);
                selective_repeat_strategy::send_packet(*(window.begin()));
                wnd_mutex.unlock();
            }
        }
    }
}

void selective_repeat_strategy::send_packet(data_packet *packet) {
    std::cout << "Sending packet with seqno = " << packet->get_seqno() << std::endl;
    bool triple_dup_ack = packet_sender::send_packet(server_socket, client_address, packet);
    packet->set_time_stamp(std::chrono::steady_clock::now());
    set_mutex.lock();
    unacked_packets.insert(packet);
    set_mutex.unlock();
    if (triple_dup_ack) {
        shrink_window(window_size / 2);
    }
}


void selective_repeat_strategy::advance_window() {
    auto pkt_iter = window.begin();
    while (!window.empty()) {
        if ((*pkt_iter)->is_acked()) {
            window.pop_front();
            std::cout << "Popped packet " << (*pkt_iter)->get_seqno() << " from window" << std::endl;
            delete *pkt_iter;
            pkt_iter = window.begin();
        } else {
            break;
        }
    }
}
