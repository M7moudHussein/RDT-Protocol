//
// Created by awalid on 12/5/18.
//

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "server.h"
#include <unistd.h>
#include "parser/server_parser.h"


server::server(server_parser serv_parser) : MAX_WINDOW_SIZE(serv_parser.get_max_window_size()) {
    server::port_number = serv_parser.get_port_number();
    server::random_seed = serv_parser.get_random_seed();
    server::loss_probability = serv_parser.get_loss_probability();
    std::cout << "Server listening on port: " << server::port_number <<  std::endl;
    server::init();
}

void server::init() {

    // Creating socket file descriptor
    if ((server::socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server::socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));

    // Filling server information
    server_address.sin_family = AF_INET; // IPv4
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(static_cast<uint16_t>(server::port_number));

    // Bind the socket with the server address
    if (bind(server::socket_fd, (const struct sockaddr *) &(server::server_address),
             sizeof(server::server_address)) < 0) {
        perror("init bind failed");
        exit(EXIT_FAILURE);
    }
}

void doSth (sockaddr_in client_addr, char *file_name) {
    int child_fd;
//    sockaddr_in child_address;
    // Creating child socket file descriptor
    if ((child_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
//    memset(&child_address, 0, sizeof(child_address));
//
//    // Filling server information
//    child_address.sin_family = AF_INET; // IPv4
//    child_address.sin_addr.s_addr = INADDR_ANY;
//    child_address.sin_port = htons(static_cast<uint16_t>(8080));

//    // Bind the socket with the server address
//    if (bind(child_fd, (const struct sockaddr *) &(client_addr),
//             sizeof(child_address)) < 0) {
//        perror("bind ffff failed");
//        exit(EXIT_FAILURE);
//    }

    // Start sending files' packets to client
    const char *data = "Data from child is here bla bla";
    sendto(child_fd, data, strlen(data),
           MSG_CONFIRM, (const struct sockaddr *) &client_addr,
           sizeof(client_addr));
    printf("Data sent to client from child.\n");

    ssize_t bytes_received;
    char buffer[1024 + 1];
    struct sockaddr_in client_address{};
    socklen_t client_address_len;
    memset(&client_address, 0, sizeof(client_address));
    bytes_received = recvfrom(child_fd, (char *)buffer, 1024,
                              MSG_WAITALL, (struct sockaddr *) &client_address,
                              &client_address_len);
    buffer[bytes_received] = '\0'; // Null terminate the buffer
    printf("Client sent this to child(Supposedly an ack): %s\n", buffer);

    // After all packets are sent, terminate child
}

void server::start() {
    while (true) {
        // Receive new client request
        ssize_t bytes_received;
        char buffer[1024 + 1];
        struct sockaddr_in client_address{};
        socklen_t client_address_len;
        memset(&client_address, 0, sizeof(client_address));
        bytes_received = recvfrom(server::socket_fd, (char *)buffer, 1024,
                                  MSG_WAITALL, (struct sockaddr *) &client_address,
                                  &client_address_len);
        buffer[bytes_received] = '\0'; // Null terminate the buffer
        std::cout << "New client request: " << buffer << std::endl;

        // Send acknowledge to client for the request
        char *hello = const_cast<char *>("Request acknowledged from parent to client");
        sendto(server::socket_fd, (const char *)hello, strlen(hello),
               MSG_CONFIRM, (const struct sockaddr *) &client_address,
               client_address_len);
        printf("Request ACK sent from parent.\n");

        // Fork a process to deal with client and send file required to it
        int pid = fork();
        if (pid < 0) {
            perror("Forking failed!");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            doSth(client_address, buffer);
        }
    }
}

void validate_args(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Invalid number of arguments!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv) {
    validate_args(argc, argv);
    server_parser serv_parser(argv[1]);
    server _server(serv_parser);
    _server.start();
    return 0;
}
