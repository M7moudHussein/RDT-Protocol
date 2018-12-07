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
    string req_datagram_buffer = create_req_datagram();

     /* Sending request to server */
    sendto(socket_fd, req_datagram_buffer.c_str(), req_datagram_buffer.length(),
           0, (const struct sockaddr *) &server_addr, sizeof(server_addr));

    /* Waiting for ACK should go here*/

    /* Waiting for new datagrams */
    receive_datagrams();
    
    /* Closing client socket */
    close(socket_fd);
}

void client::init() {
    /* Creating socket file descriptor */
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    /* Setting client address */
    memset(&local_addr, 0, sizeof(local_addr));

    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(parser.get_local_port_no());

    /* Binding the client socket fd to an address with the given local port number */
    if (bind(socket_fd, (const struct sockaddr *) &local_addr, sizeof(local_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    /* Setting server address */
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(parser.get_server_ip().c_str());
    server_addr.sin_port = htons(parser.get_server_port_no());
}

string client::create_req_datagram() {
    string data = parser.get_req_file_name();
    data_packet pkt(data);
    pkt.set_len(static_cast<uint16_t>(data.length()));
    pkt.set_seqno(0);
    pkt.set_cksum(calculate_checksum(pkt)); // last step after setting all headers
    std::stringstream pkt_buffer;
    pkt_buffer << pkt;
    return pkt_buffer.str();
}

void client::receive_datagrams() {

}


int main(int argc, char **argv) {
    if (argc != 2) {
        perror("INVALID ARGUMENTS");
        exit(EXIT_FAILURE);
    }
    client client(argv[1]);
    client.run();
}