//
// Created by awalid on 12/8/18.
//

#ifndef RDT_PROTOCOL_CLIENT_RDT_STRATEGY_H
#define RDT_PROTOCOL_CLIENT_RDT_STRATEGY_H

#include <netinet/in.h>
#include <set>
#include "../../shared/ack_packet.h"
#include "../../shared/data_packet.h"
#include "../../shared/packet_parser.h"
#include <sstream>

#define BUF_SIZE 512

class client_rdt_strategy {
public:
    virtual void run() = 0;

    void set_server_address(sockaddr_in server_address) {
        client_rdt_strategy::server_address = server_address;
    }

    void set_client_socket(int client_socket) {
        client_rdt_strategy::client_socket = client_socket;
    }

    bool is_done() {
        // TODO: return true if end of file reached
        return false;
    }

protected:
    void send_ack(ack_packet *packet) {
        std::stringstream ack_ss;
        ack_ss << packet;
        sendto(client_socket, ack_ss.str().c_str(), ack_ss.str().length(),
               MSG_CONFIRM, (const struct sockaddr *) &server_address, sizeof(server_address));
    }

    void advance_window() {

    }

    packet_parser parser;
    sockaddr_in server_address;
    int client_socket;
    std::set<data_packet> window; // Add comparator by seqno
    int window_size;
    int expected_seqno;
    std::string file_data;
};

#endif //RDT_PROTOCOL_CLIENT_RDT_STRATEGY_H
