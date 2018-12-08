#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include "client.h"
#include "parser/client_parser.h"
#include "../shared/packet_util.h"
#include "../shared/data_packet.h"

client::client(string args_file_path) : parser(args_file_path) {
    init();
}

void client::run() {
    /* Creating request datagram */
    req_datagram_buffer = create_req_datagram();

    /* Sending request to server */
    sendto(socket_fd, req_datagram_buffer.c_str(), req_datagram_buffer.length(),
           0, (const struct sockaddr *) &server_addr, sizeof(server_addr));

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

    /*init ack pkt for file request */
    ack_pkt = new ack_packet();
}

string client::create_req_datagram() {
    string data = parser.get_req_file_name();
    data_packet pkt(data);
    pkt.set_len(static_cast<uint16_t>(HEADER_SIZE + data.length()));
    pkt.set_seqno(0);
    pkt.set_cksum(calculate_checksum(pkt)); // last step after setting all headers
    std::stringstream pkt_buffer;
    pkt_buffer << pkt;
    return pkt_buffer.str();
}

void client::receive_datagrams() {
    uint32_t seq_no = 0;
    uint32_t n = 2;

}

void client::handle_ack_timeout() {
    // while ack not received try resending request again
    while (true) {
        fd_set rfds;
        struct timeval tv;
        int retval;
        /* Watch stdin (fd 0) to see when it has input. */
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        /* Wait up to five seconds for ACK to arrive. */
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        retval = select(1, &rfds, NULL, NULL, &tv);

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
                   0, (const struct sockaddr *) &server_addr, sizeof(server_addr));
        }
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