#ifndef RDT_PROTOCOL_CLIENT_H
#define RDT_PROTOCOL_CLIENT_H

#include <netinet/in.h>

#include "parser/client_parser.h"
#include "../server/timer_thread.h"
#include "../shared/ack_packet.h"

using std::string;

class client {
public:
    client(string args_file_path);

    void run();

private:
    enum mode {
        STOP_AND_WAIT,
        SELECTIVE_REPEAT,
        GO_BACK_N
    };

    client_parser parser;
    int socket_fd;
    struct sockaddr_in local_addr;
    struct sockaddr_in server_addr;
    string req_datagram_buffer;
    mode mode;

    void init();

    void set_mode(string mode_str);

    string create_req_datagram();

    void handle_ack_timeout();

    void receive_datagrams();

    bool is_server_addr(sockaddr_in);
};


#endif
