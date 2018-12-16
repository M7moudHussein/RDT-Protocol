#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "server.h"
#include <sstream>
#include "strategy/rdt_strategy.h"
#include "strategy/selective_repeat_strategy.h"
#include "strategy/go_back_N_strategy.h"

#define MAX_UDP_BUFFER_SIZE 65536

server::server(server_parser serv_parser) : MAX_WINDOW_SIZE(serv_parser.get_max_window_size()) {
    server::port_number = serv_parser.get_port_number();

    packet_sender::set_seed(serv_parser.get_random_seed());
    packet_sender::set_probability(serv_parser.get_loss_probability());
    packet_sender::set_loss_sequence(serv_parser.get_loss_sequence());

    set_mode(serv_parser.get_server_mode());
    std::cout << "Server listening on port: " << server::port_number << std::endl;
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
                   &opt, sizeof(opt))) {
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
    char buffer[MAX_UDP_BUFFER_SIZE + 1];
    while (true) {
        // Receives client packets
        ssize_t bytes_received;
        struct sockaddr_in client_address{};
        socklen_t client_address_len = sizeof(client_address);
        bytes_received = recvfrom(server::socket_fd, buffer, MAX_UDP_BUFFER_SIZE,
                                  MSG_WAITALL, (struct sockaddr *) &client_address,
                                  &client_address_len);
//        std::cout << "Server received " << bytes_received << " bytes" << std::endl;

        std::string client_address_string = get_address_string(client_address);
        if (server::registered_clients.find(client_address_string) != server::registered_clients.end()) {
            ack_packet ack = ack_packet(buffer);
//            std::cout << "Main thread: " << ack.get_ackno() << std::endl;
            // Push received ack in the queue of the thread handling this client
            server::registered_clients_mtx.lock();
            std::thread::id worker_to_ack = server::registered_clients[client_address_string];
            server::registered_clients_mtx.unlock();

            server::worker_threads_acks_mtx.lock();
            server::worker_threads_acks[worker_to_ack].push(ack);
            server::worker_threads_acks_mtx.unlock();
        } else {
            data_packet request = data_packet(buffer, static_cast<int>(bytes_received));
            ack_packet ack = ack_packet();
            sendto(server::socket_fd, ack.pack().c_str(), ack.pack().length(),
                   MSG_CONFIRM, (const struct sockaddr *) &client_address,
                   client_address_len);

            // Dispatch worker thread
            server::dispatch_worker_thread(client_address, request.get_data());
        }
    }
}

void server::cleanup_working_threads() {
//    while (true) {
//        server::working_threads_mtx.lock();
//        for (auto it = server::working_threads.cbegin();
//             it != server::working_threads.cend();) {
//            if (it->second->is_done()) {
//                // Removing entry from worker_threads_acks map
//                server::worker_threads_acks_mtx.lock();
//                server::working_threads.erase(it->first);
//                server::worker_threads_acks_mtx.unlock();
//
//                // Removing entry from registered clients map (done in O(n) because tid is value not key)
//                server::registered_clients_mtx.lock();
//                for (auto rc_it = server::registered_clients.cbegin();
//                     rc_it != server::registered_clients.cend();) {
//                    if (rc_it->second == it->first) {
//                        server::registered_clients.erase(rc_it); // delete pointer to it from map
//                        break;
//                    }
//                }
//
//                // Removing entry from working threads map
//                delete it->second; // delete worker thread object
//                server::working_threads.erase(it++); // delete pointer to it from map
//            } else
//                it++;
//        }
//        server::working_threads_mtx.unlock();
//        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Sleep for 500 ms
//    }
}

void server::dispatch_worker_thread(sockaddr_in client_address, std::string file_path) {
    std::thread *th = new std::thread(&server::handle_worker_thread, this, client_address, file_path);
    worker_thread *wrk_th = new worker_thread(th);

    server::registered_clients_mtx.lock();
    server::registered_clients[get_address_string(client_address)] = wrk_th->get_thread_id();
    server::registered_clients_mtx.unlock();

    server::working_threads_mtx.lock();
    server::working_threads[wrk_th->get_thread_id()] = wrk_th;
    server::working_threads_mtx.unlock();

    wrk_th->detach();
}


void server::handle_worker_thread(sockaddr_in client_address, std::string file_path) {
    rdt_strategy *rdt = nullptr;

    switch (this->server_mode) {
        case STOP_AND_WAIT:
            rdt = new selective_repeat_strategy(file_path, 1, 1);
            break;
        case SELECTIVE_REPEAT:
            rdt = new selective_repeat_strategy(file_path, MAX_WINDOW_SIZE);
            break;
        case GO_BACK_N:
            rdt = new go_back_N_strategy(file_path);
            break;
        default:
            perror("Invalid mode");
            exit(EXIT_FAILURE);
    }

    rdt->set_server_socket(server::socket_fd);
    rdt->set_client_address(client_address);

    rdt->start();

    while (!rdt->is_done()) {
        std::thread::id worker_id = std::this_thread::get_id();
        ack_packet ack;
        while (true) {
            this->worker_threads_acks_mtx.lock();
            if (!this->worker_threads_acks[worker_id].empty()) {
                ack = this->worker_threads_acks[worker_id].front();
//                std::cout << "Worker thread ACK: " << ack.get_ackno() << std::endl;
                this->worker_threads_acks[worker_id].pop();
                this->worker_threads_acks_mtx.unlock();
                break;
            }
            this->worker_threads_acks_mtx.unlock();
        }

        rdt->acknowledge_packet(ack);
    }
    std::cout << "Worker thread for client with address = " << get_address_string(client_address) << " is done"
              << std::endl;

    //Send empty data packet to inform client that file transfer is complete
    rdt->send_empty_packet();

    // After all client handling is finished, mark yourself as done to be cleaned up by cleanup thread
    server::finalize_worker_thread();
}


void server::set_mode(std::string mode_str) {
    if (mode_str == "selective_repeat") {
        server::server_mode = SELECTIVE_REPEAT;
    } else if (mode_str == "go_back_n") {
        server::server_mode = GO_BACK_N;
    } else if (mode_str == "stop_and_wait") {
        server::server_mode = STOP_AND_WAIT;
    } else {
        std::cerr << "Mode not supported!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void server::finalize_worker_thread() {
    server::working_threads_mtx.lock();
    server::working_threads[std::this_thread::get_id()]->mark_done();
    server::working_threads_mtx.unlock();
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
