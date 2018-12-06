//
// Created by awalid on 12/5/18.
//

#ifndef RDT_PROTOCOL_SERVER_H
#define RDT_PROTOCOL_SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "parser/server_parser.h"

class server {
private:
    enum mode {
        STOP_AND_WAIT,
        SELECTIVE_REPEAT,
        GO_BACK_N
    };
    int socket_fd;
    struct sockaddr_in server_address;
    int port_number;
    int random_seed;
    float loss_probability;

    void init();

public:
    const int MAX_WINDOW_SIZE;

    explicit server(server_parser serv_parser);
    void set_server_mode();
    void start();



};


#endif //RDT_PROTOCOL_SERVER_H
