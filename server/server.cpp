//
// Created by awalid on 12/5/18.
//

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "server.h"
#include <unistd.h>
#include <sstream>
#include "parser/server_parser.h"
#include "../shared/ack_packet.h"


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

    // Initiate working threads cleanup service
    std::thread *cleanup_th = new std::thread(&server::cleanup_working_threads, this);
    cleanup_th->detach();
}

std::string get_address_string(sockaddr_in address) {
    // Returns address in string form to be used in map (form -> ip:port)
    char dst[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(address.sin_addr), dst, INET_ADDRSTRLEN);
    return std::string(dst) + std::string(":") + std::string(std::to_string(ntohs(address.sin_port)));
}

void server::start() {
    while (true) {
        // Receives client packets
        ssize_t bytes_received;
        char buffer[1024 + 5];
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof client_address;
        bytes_received = recvfrom(server::socket_fd, (char *)buffer, 1024,
                                  MSG_WAITALL, (struct sockaddr *) &client_address,
                                  &client_address_len);
        buffer[bytes_received + 1] = '\0'; // Null terminate the buffer
        std::cout << "Client message received: " << buffer << std::endl;

        // Parse message in buffer and form a packet

        // Check if client_address is registered or not
        if (server::registered_clients.find(get_address_string(client_address)) != server::registered_clients.end()) {
            // Client already registered, notify corresponding worker thread
        } else {
            // Acknowledge receiving of request
            ack_packet ack;
            std::cout << ack << std::endl;
            std::stringstream ack_ss;
            ack_ss << ack;
            sendto(server::socket_fd, ack_ss.str().c_str(), ack_ss.str().length(),
                   MSG_CONFIRM, (const struct sockaddr *) &client_address,
                   client_address_len);

            // Create new worker thread for the client

            // Add thread to working threads, add client to registered clients, add false ack_received bool in threads_acks
        }
    }
}

void server::cleanup_working_threads() {
    while (true) {
        server::working_threads_mtx.lock();
        for (auto it = server::working_threads.cbegin();
             it != server::working_threads.cend();) {
            if (it->second->is_done()) {
                delete it->second; // delete worker thread object
                server::working_threads.erase(it++); // delete pointer to it from map
            } else
                it++;
        }
        server::working_threads_mtx.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Sleep for 500 ms
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
