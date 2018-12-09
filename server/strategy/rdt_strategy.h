#ifndef RDT_PROTOCOL_RDT_STRATEGY_H
#define RDT_PROTOCOL_RDT_STRATEGY_H

#include <netinet/in.h>
#include <thread>
#include <sstream>
#include "../../shared/ack_packet.h"
#include "../filereader/packet_builder.h"
#include "../timer_thread.h"

class rdt_strategy {
public:
    virtual void acknowledge_packet(ack_packet ack_pkt) = 0;

    virtual void start() = 0;

    void set_client_address(sockaddr_in client_address) {
        this->client_address = client_address;
    }

    void set_server_socket(int server_socket) {
        this->server_socket = server_socket;
    }

    bool is_done() {
        return this->window.empty();
    }


protected:
    void fill_window() {
        while (window.size() < window_size && pkt_builder->has_next()) {
            window.push_back(pkt_builder->get_next_packet());
        }
    }

    void advance_window() {
        for (auto it = window.begin(); it != window.end() && (*it)->is_acked(); it++) {
            window.pop_front();
            if (pkt_builder->has_next())
                window.push_back(pkt_builder->get_next_packet());
        }
    }

    virtual void handle_time_out() = 0;

    virtual void send_packet(data_packet *packet) = 0;

    packet_builder *pkt_builder;
    sockaddr_in client_address;
    int server_socket;
    timer_thread *timer;
    const std::chrono::seconds PACKET_TIME_OUT = std::chrono::seconds(5); // assumed time out to be 5 seconds
    std::set<data_packet *> unacked_packets;
    std::mutex set_mutex;
    std::deque<data_packet *> window;
    int window_size;
};

#endif //RDT_PROTOCOL_RDT_STRATEGY_H
