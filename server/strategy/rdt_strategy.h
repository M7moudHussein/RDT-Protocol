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
    virtual void acknowledge_packet(ack_packet &ack_pkt) = 0;

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

    void send_empty_packet() {
        // Assuming here no loss in sending this empty packet
        auto packet = new data_packet(std::string(), next_seq_number);
        sendto(server_socket, packet->pack().c_str(), packet->pack().length(),
               0, (const struct sockaddr *) &client_address,
               sizeof client_address);
    }


protected:
    void fill_window() {
        while (window.size() < window_size && pkt_builder->has_next()) {
            window.push_back(pkt_builder->get_next_packet(next_seq_number));
        }
    }

    virtual void advance_window() = 0;

    virtual void handle_time_out() = 0;

    virtual void send_packet(data_packet *packet) = 0;

    packet_builder *pkt_builder;
    sockaddr_in client_address;
    int server_socket;
    timer_thread *timer;
    std::set<data_packet *, data_packet::time_comparator> unacked_packets;
    std::mutex set_mutex;
    std::mutex wnd_mutex;
    std::deque<data_packet *> window;
    std::deque<data_packet *> aux_window;
    int window_size, next_seq_number;
    int max_window_size;
    int threshold;
};

#endif //RDT_PROTOCOL_RDT_STRATEGY_H
