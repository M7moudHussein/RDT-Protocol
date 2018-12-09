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

client::client(string args_file_path) : parser(args_file_path) {
    init();
}

void client::run() {
    /* Creating request datagram */
    req_datagram_buffer = create_req_datagram();

    /* Sending request to server */
    sendto(socket_fd, req_datagram_buffer.c_str(), req_datagram_buffer.length(),
           MSG_CONFIRM, (const struct sockaddr *) &server_addr, sizeof(server_addr));

    /* Waiting for ACK and resending req datagram*/
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

    /*init ack pkt for file request */
    ack_pkt = new ack_packet();
}

string client::create_req_datagram() {
    string data = parser.get_req_file_name();
    std::cout << "Data here fams: " << data << std::endl;
    data_packet *pkt = new data_packet(data);
    pkt->set_len(static_cast<uint16_t>(HEADER_SIZE + data.length()));
    pkt->set_seqno(static_cast<uint32_t>(0));
    pkt->set_cksum(packet_util::calculate_checksum(pkt)); // last step after setting all headers
    return pkt->pack();
}

void client::receive_datagrams() {
    client_rdt_strategy *rdt = nullptr;
    switch (client::client_mode) {
        case STOP_AND_WAIT:
            rdt = new client_selective_repeat_strategy(1);
            break;
        case SELECTIVE_REPEAT:
            rdt = new client_selective_repeat_strategy(client::parser.get_default_window_size());
            break;
        case GO_BACK_N:
            rdt = new client_go_back_N_strategy(client::parser.get_default_window_size());
            break;
        default:
            perror("Invalid mode");
            exit(EXIT_FAILURE);
    }

    rdt->set_client_socket(client::socket_fd);
    rdt->set_server_address(client::server_addr);

    while (!rdt->is_done()) {
        rdt->run();
    }
}

void client::handle_ack_timeout() {
    // while ack not received try resending request again
    while (true) {
        fd_set rfds;
        struct timeval tv;
        int retval;
        /* Watch stdin (fd 0) to see when it has input. */
        FD_ZERO(&rfds);
        FD_SET(socket_fd, &rfds);
        /* Wait up to five seconds for ACK to arrive. */
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        retval = select(socket_fd + 1, &rfds, NULL, NULL, &tv);

        if (retval == -1)
            perror("select()");
        else if (retval){
            printf("ACK for file request received ! \n");
            /* FD_ISSET(0, &rfds) will be true. */
            break;
        }
        else {
            printf("No ACK received within five seconds..... request packet resending\n");
            /* Resending request to server */
            sendto(socket_fd, req_datagram_buffer.c_str(), req_datagram_buffer.length(),
                   MSG_CONFIRM, (const struct sockaddr *) &server_addr, sizeof(server_addr));
        }
    }
}

void client::set_mode(std::string mode_str) {
    if (mode_str == "selective_repeat") {
        client::client_mode = SELECTIVE_REPEAT;
    } else if (mode_str == "go_back_n") {
        client::client_mode = GO_BACK_N;
    } else if (mode_str == "stop_and_wait") {
        client::client_mode = STOP_AND_WAIT;
    } else {
        std::cerr << "Mode not supported!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        perror("INVALID ARGUMENTS");
        exit(EXIT_FAILURE);
    }
    client client(argv[1]);
    client.run();
}