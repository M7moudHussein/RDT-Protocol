//
// Created by awalid on 12/5/18.
//

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "server.h"

void server::init() {

    // Creating socket file descriptor
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    // Filling server information
    server_address.sin_family = AF_INET; // IPv4
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port_number);

    // Bind the socket with the server address
    if (bind(socket_fd, (const struct sockaddr *) &server_address,
             sizeof(server_address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

}
