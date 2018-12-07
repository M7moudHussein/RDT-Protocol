#ifndef RDT_PROTOCOL_RDT_STRATEGY_H
#define RDT_PROTOCOL_RDT_STRATEGY_H

#include <netinet/in.h>
#include <thread>
#include <sstream>
#include "../shared/ack_packet.h"
#include "filereader/packet_builder.h"
#include "timer_thread.h"

class rdt_strategy {
public:
    virtual void acknowledge_packet(ack_packet ack_pkt) = 0;

    virtual void start() = 0;

    void set_client_address(sockaddr_in client_address);

    void set_server_socket(int server_socket);

    bool is_done();

    void handle_time_out();

    virtual void send_packet(data_packet *packet) = 0;

protected:
    packet_builder *pkt_builder;
    sockaddr_in client_address;
    int server_socket;
    timer_thread *timer;
    const std::chrono::seconds PACKET_TIME_OUT = std::chrono::seconds(5); // assumed time out to be 5 seconds
    std::set<data_packet *, data_packet::time_comparator> unacked_packets;
    std::mutex set_mutex;
    std::deque<data_packet *> window;
    int window_size;
};

void rdt_strategy::set_client_address(sockaddr_in client_address) {
    this->client_address = client_address;
}

bool rdt_strategy::is_done() {
    return this->window.size() == 0;
}

void rdt_strategy::handle_time_out() {
    while (true) {
        set_mutex.lock();
        data_packet *first_unacked_pkt = *(unacked_packets.begin());
        set_mutex.unlock();
        if (first_unacked_pkt->get_time_stamp() + PACKET_TIME_OUT < std::chrono::steady_clock::now())
            timer->sleep_until(first_unacked_pkt->get_time_stamp() + PACKET_TIME_OUT);
        else{
            set_mutex.lock();
            unacked_packets.erase(first_unacked_pkt);
            rdt_strategy::send_packet(first_unacked_pkt); //TODO check rdt::send_pkt implemented in child class

        }
    }
}

void rdt_strategy::set_server_socket(int server_socket) {
    this->server_socket = server_socket;
}

#endif //RDT_PROTOCOL_RDT_STRATEGY_H
