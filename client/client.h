#ifndef RDT_PROTOCOL_CLIENT_H
#define RDT_PROTOCOL_CLIENT_H

#include <netinet/in.h>

#include "parser/client_parser.h"
#include "../shared/datagram.h"

using std::string;

class client {
public:
    client(string args_file_path);

    void run();

private:
    client_parser parser;
    int socket_fd;
    struct sockaddr_in local_addr;
    struct sockaddr_in server_addr;

    void init();

    string create_req_datagram();

    void receive_datagrams();
};


#endif
