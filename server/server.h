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
#include <queue>
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

    int socket_fd;
    sockaddr_in server_address;
    int port_number;
    mode server_mode;

    std::map<std::thread::id, worker_thread *> working_threads;
    std::mutex working_threads_mtx;

    std::map<std::string, std::thread::id> registered_clients;
    std::mutex registered_clients_mtx;

    std::map<std::thread::id, std::queue<ack_packet>> worker_threads_acks;
    std::mutex worker_threads_acks_mtx;

    void dispatch_worker_thread(sockaddr_in client_address, std::string file_path);

    void handle_worker_thread(sockaddr_in client_address, std::string file_path);

    void finalize_worker_thread();

    void cleanup_working_threads();

    void init();

    void set_mode(std::string mode_str);


public:
    const int MAX_WINDOW_SIZE;

    explicit server(server_parser serv_parser);

    void start();
};


#endif //RDT_PROTOCOL_SERVER_H
