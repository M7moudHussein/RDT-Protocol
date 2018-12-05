//
// Created by awalid on 12/5/18.
//

#ifndef RDT_PROTOCOL_SERVER_H
#define RDT_PROTOCOL_SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

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

public:
    void set_server_mode();

    void init();


};


#endif //RDT_PROTOCOL_SERVER_H
