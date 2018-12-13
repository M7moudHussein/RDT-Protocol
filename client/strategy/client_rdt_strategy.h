
#ifndef RDT_PROTOCOL_CLIENT_RDT_STRATEGY_H
#define RDT_PROTOCOL_CLIENT_RDT_STRATEGY_H

#include <netinet/in.h>
#include <set>
#include "../../shared/ack_packet.h"
#include "../../shared/data_packet.h"
#include <sstream>

class client_rdt_strategy {
public:
    virtual void run() = 0;

    void set_client_socket(int client_socket) {
        client_rdt_strategy::client_socket = client_socket;
    }

    void set_server_address(sockaddr_in server_address) {
        client_rdt_strategy::server_address = server_address;
    }

    bool is_done() {
        return done;
    }

protected:
    sockaddr_in server_address;
    int client_socket;

    std::set<data_packet, data_packet::seq_num_comparator> window;
    int window_size;
    uint32_t expected_seqno;

    std::string file_data;

    bool done;

    void send_ack(ack_packet *packet) {
        std::string pkt = packet->pack();
        sendto(client_socket, pkt.c_str(), pkt.length(), MSG_CONFIRM,
               (const struct sockaddr *) &server_address, sizeof(server_address));
    }

    bool is_terminal_pkt(data_packet *packet) {
        return packet->get_data().length() == 0;
    }

    void advance_window() {

    }
};

#endif
