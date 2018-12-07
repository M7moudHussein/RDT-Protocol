#ifndef RDT_PROTOCOL_RDT_STRATEGY_H
#define RDT_PROTOCOL_RDT_STRATEGY_H

#include <netinet/in.h>
#include <thread>
#include "../shared/ack_packet.h"
#include "filereader/packet_builder.h"
#include "timer_thread.h"
class rdt_strategy {
public:
    virtual void acknowledge_packet(ack_packet ack_pkt) = 0;

    virtual void start() = 0;

    void set_client_address(sockaddr_in client_address);

    bool is_done();

protected:
    packet_builder *pkt_builder;
    sockaddr_in client_address;
    bool _is_done;
    timer_thread *timer;
    const std::chrono::seconds PACKET_TIME_OUT = std::chrono::seconds(5); // assumed time out to be 5 seconds
    std::set<data_packet *, data_packet::time_comparator> unacked_packets;
    std::queue<data_packet *> window;
    int window_size;
};

void rdt_strategy::set_client_address(sockaddr_in client_address) {
    this->client_address = client_address;
}

bool rdt_strategy::is_done() {
    return this->_is_done;
}

#endif //RDT_PROTOCOL_RDT_STRATEGY_H
