//
// Created by awalid on 12/5/18.
//

#ifndef RDT_PROTOCOL_SERVER_H
#define RDT_PROTOCOL_SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <thread>
#include <mutex>
#include <map>
#include <set>
#include <condition_variable>
#include "parser/server_parser.h"
#include "worker_thread.h"
#include "../shared/data_packet.h"
#include "../shared/ack_packet.h"

class server {
private:
    enum mode {
        STOP_AND_WAIT,
        SELECTIVE_REPEAT,
        GO_BACK_N
    };

    struct worker_controls {
        std::mutex m;
        std::condition_variable cv;
        bool ack_arrived;
        ack_packet ack;
    };

    int socket_fd;
    sockaddr_in server_address;
    int port_number;
    int random_seed;
    float loss_probability;
    mode server_mode;

    std::map<std::thread::id, worker_thread *> working_threads;
    std::mutex working_threads_mtx;

    std::map<std::string, std::thread::id> registered_clients;
    std::mutex registered_clients_mtx;

    std::map<std::thread::id, worker_controls *> worker_threads_acks;
    std::mutex worker_threads_acks_mtx;

    void dispatch_worker_thread(sockaddr_in client_address, std::string file_path);

    void handle_worker_thread(sockaddr_in client_address, std::string file_path);

    void finalize_worker_thread();

    void cleanup_working_threads();

    void init();


public:
    const int MAX_WINDOW_SIZE;

    explicit server(server_parser serv_parser);

    void start();

    data_packet *get_first_unacked_packet();


};


#endif //RDT_PROTOCOL_SERVER_H
