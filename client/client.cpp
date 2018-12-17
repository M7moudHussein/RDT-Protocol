#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include "client.h"
#include "parser/client_parser.h"
#include "../shared/packet_util.h"
#include "../shared/data_packet.h"
#include "strategy/client_rdt_strategy.h"
#include "strategy/client_go_back_N_strategy.h"
#include "strategy/client_selective_repeat_strategy.h"
#include "filewriter/file_writer.h"

client::client(string args_file_path) : parser(args_file_path) {
    init();
}

void client::run() {
    /* Creating request datagram */
    req_datagram_buffer = create_req_datagram();

    /* Sending request to server */
    sendto(socket_fd, req_datagram_buffer.c_str(), req_datagram_buffer.length(),
           0, (const struct sockaddr *) &server_addr, sizeof(server_addr));

    /* Waiting for ACK and resending req datagram */
    handle_ack_timeout();

    /* Waiting for new datagrams */
    receive_datagrams();

    /* Closing client socket */
    close(socket_fd);
}

void client::init() {
    /* Creating socket file descriptor */
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("SOCKET CREATION FAILED");
        exit(EXIT_FAILURE);
    }

    /* Setting client address */
    memset(&local_addr, 0, sizeof(local_addr));

    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(parser.get_local_port_no());

    /* Binding the client socket fd to an address with the given local port number */
    if (bind(socket_fd, (const struct sockaddr *) &local_addr, sizeof(local_addr)) < 0) {
        perror("BINDING FAILED");
        exit(EXIT_FAILURE);
    }

    /* Setting server address */
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(parser.get_server_ip().c_str());
    server_addr.sin_port = htons(parser.get_server_port_no());

    /* Setting mode of client */
    set_mode(parser.get_client_mode());
}

void client::set_mode(string mode_str) {
    if (mode_str == "stop_and_wait") {
        mode = STOP_AND_WAIT;
    } else if (mode_str == "selective_repeat") {
        mode = SELECTIVE_REPEAT;
    } else if (mode_str == "go_back_n") {
        mode = GO_BACK_N;
    } else {
        std::cerr << "Mode not supported!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

string client::create_req_datagram() {
    string data = parser.get_req_file_name();
    std::cout << "Filename requested: " << data << std::endl;
    data_packet *pkt = new data_packet(data, 0);
    return pkt->pack();
}

void client::handle_ack_timeout() {
    while (true) {
        fd_set rfds;
        struct timeval tv;
        int retval;

        FD_ZERO(&rfds);
        FD_SET(socket_fd, &rfds);

        tv.tv_sec = 5; // Wait up to five seconds for ACK to arrive
        tv.tv_usec = 0;

        retval = select(socket_fd + 1, &rfds, NULL, NULL, &tv);

        if (retval == -1) { // error
            perror("SELECT FAILED");
            exit(EXIT_FAILURE);
        } else if (retval) { // data received
            printf("ACK received for file request!\n");
            struct sockaddr_in sender_addr;
            socklen_t sender_addr_len = sizeof(sender_addr);
            ssize_t bytes_received = recvfrom(socket_fd, new char[HEADER_SIZE], HEADER_SIZE, MSG_WAITALL,
                                              (struct sockaddr *) &sender_addr, &sender_addr_len);
            if (is_server_addr(sender_addr)) // TODO: add this check after each call to recvfrom() in all strategies?
                break;
        } else { // timeout -> resend request packet
            printf("NO ACK RECEIVED WITHIN 5 SECONDS\n");
            printf("RESENDING REQUEST.....\n");
            sendto(socket_fd, req_datagram_buffer.c_str(), req_datagram_buffer.length(),
                   0, (const struct sockaddr *) &server_addr, sizeof(server_addr));
        }
    }
}

void client::receive_datagrams() {
    client_rdt_strategy *rdt = nullptr;
    switch (mode) {
        case STOP_AND_WAIT:
            rdt = new client_selective_repeat_strategy(1);
            break;
        case SELECTIVE_REPEAT:
            rdt = new client_selective_repeat_strategy(parser.get_default_window_size());
            break;
        case GO_BACK_N:
            rdt = new client_go_back_N_strategy(parser.get_default_window_size());
            break;
        default:
            perror("INVALID MODE");
            exit(EXIT_FAILURE);
    }

    rdt->set_client_socket(socket_fd);
    rdt->set_server_address(server_addr);

    // open file to write requested file data in
    file_writer::open(parser.get_req_file_name());
   //file_writer::open("output");


    while (!rdt->is_done())
        rdt->run();

    std::cout << "File received fully from server" << std::endl;

    // After all packets received close the file
    file_writer::close();
}

bool client::is_server_addr(sockaddr_in sender_addr) {
    bool same_family = sender_addr.sin_family == server_addr.sin_family;
    bool same_ip = sender_addr.sin_addr.s_addr == server_addr.sin_addr.s_addr;
    bool same_port = sender_addr.sin_port == server_addr.sin_port;
    return same_family && same_ip && same_port;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        perror("INVALID ARGUMENTS");
        exit(EXIT_FAILURE);
    }
    client client(argv[1]);
    client.run();
}