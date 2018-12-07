//
// Created by awalid on 12/5/18.
//

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "server.h"
#include <sstream>
#include "rdt_strategy.h"
#include "selective_repeat_strategy.h"
#include "timer_thread.h"

#define MAX_UDP_BUFFER_SIZE 65536

server::server(server_parser serv_parser) : MAX_WINDOW_SIZE(serv_parser.get_max_window_size()) {
    server::port_number = serv_parser.get_port_number();
    server::random_seed = serv_parser.get_random_seed();
    server::loss_probability = serv_parser.get_loss_probability();
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
    char *buffer = new char[MAX_UDP_BUFFER_SIZE + 1];
    while (true) {
        // Receives client packets
        ssize_t bytes_received;
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof client_address;
        bytes_received = recvfrom(server::socket_fd, buffer, MAX_UDP_BUFFER_SIZE,
                                  MSG_WAITALL, (struct sockaddr *) &client_address,
                                  &client_address_len);
        buffer[bytes_received + 1] = '\0'; // Null terminate the buffer
        std::cout << "Client message received: " << buffer << std::endl;

        // TODO: Parse message in buffer and form a packet

        // Check if client_address is registered or not
        std::string client_address_string = get_address_string(client_address);
        if (server::registered_clients.find(client_address_string) != server::registered_clients.end()) {
            ack_packet ack; //TODO set with the ack which arrived
            // Client already registered, change corresponding bool to true

            server::registered_clients_mtx.lock();
            std::thread::id worker_to_ack = server::registered_clients[client_address_string];
            server::registered_clients_mtx.unlock();

            server::worker_threads_acks_mtx.lock();
            worker_controls *controls = server::worker_threads_acks[worker_to_ack];
            {
                std::lock_guard<std::mutex> lk(controls->m);
                controls->ack_arrived = true;
                controls->ack = ack;
            }
            controls->cv.notify_one();
            server::worker_threads_acks_mtx.unlock();
        } else {
            // Acknowledge receiving of request
            ack_packet ack;
            std::cout << ack << std::endl;
            std::stringstream ack_ss;
            ack_ss << ack;
            sendto(server::socket_fd, ack_ss.str().c_str(), ack_ss.str().length(),
                   MSG_CONFIRM, (const struct sockaddr *) &client_address,
                   client_address_len);

            // Dispatch worker thread
            server::dispatch_worker_thread(client_address,
                                           ""); //TODO change the string to the valid file path (relative path)
        }
    }
}

void server::cleanup_working_threads() {
    while (true) {
        server::working_threads_mtx.lock();
        for (auto it = server::working_threads.cbegin();
             it != server::working_threads.cend();) {
            if (it->second->is_done()) {
                // Removing entry from worker_threads_acks map
                server::worker_threads_acks_mtx.lock();
                delete server::worker_threads_acks[it->first];
                server::working_threads.erase(it->first);
                server::worker_threads_acks_mtx.unlock();

                // Removing entry from registered clients map (done in O(n) because tid is value not key)
                server::registered_clients_mtx.lock();
                for (auto rc_it = server::registered_clients.cbegin();
                     rc_it != server::registered_clients.cend();) {
                    if (rc_it->second == it->first) {
                        server::registered_clients.erase(rc_it); // delete pointer to it from map
                        break;
                    }
                }

                // Removing entry from working threads map
                delete it->second; // delete worker thread object
                server::working_threads.erase(it++); // delete pointer to it from map
            } else
                it++;
        }
        server::working_threads_mtx.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Sleep for 500 ms
    }
}

void server::dispatch_worker_thread(sockaddr_in client_address, std::string file_path) {
    std::thread *th = new std::thread(&server::handle_worker_thread, this, client_address, file_path);
    worker_thread *wrk_th = new worker_thread(th);

    server::registered_clients_mtx.lock();
    server::registered_clients[get_address_string(client_address)] = wrk_th->get_thread_id();
    server::registered_clients_mtx.unlock();

    server::worker_threads_acks_mtx.lock();
    server::worker_threads_acks[wrk_th->get_thread_id()] = new worker_controls();
    server::worker_threads_acks_mtx.unlock();

    server::working_threads_mtx.lock();
    server::working_threads[wrk_th->get_thread_id()] = wrk_th;
    server::working_threads_mtx.unlock();

    wrk_th->detach();
}


void server::handle_worker_thread(sockaddr_in client_address, std::string file_path) {
    rdt_strategy *rdt = nullptr;

    switch (this->server_mode) {
        case STOP_AND_WAIT:
            rdt = new selective_repeat_strategy(file_path, 0);
            break;
        case SELECTIVE_REPEAT:
            rdt = new selective_repeat_strategy(file_path);
            break;
        case GO_BACK_N:
            //TODO to be handled
            break;
        default:
            perror("Invalid mode");
            exit(EXIT_FAILURE);
    }

    rdt->set_client_address(client_address);

    rdt->start();

    while (!rdt->is_done()) {
        std::thread::id worker_id = std::this_thread::get_id();
        this->worker_threads_acks_mtx.lock();
        worker_controls *controls = this->worker_threads_acks[worker_id];
        this->worker_threads_acks_mtx.unlock();

        std::unique_lock<std::mutex> lk(controls->m);
        controls->cv.wait(lk, [controls] { return controls->ack_arrived; });

        rdt->acknowledge_packet(controls->ack);
        lk.unlock();
    }

    // After all client handling is finished, mark yourself as done to be cleaned up by cleanup thread
    server::finalize_worker_thread();
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
