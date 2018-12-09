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
    ack_packet *ack_pkt;
    mode client_mode;

private:

    void init();

    string create_req_datagram();

    void receive_datagrams();

    void handle_ack_timeout();

    void set_mode(std::string mode_str);
};


#endif
