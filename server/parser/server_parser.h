//
// Created by awalid on 12/5/18.
//

#ifndef RDT_PROTOCOL_SERVER_PARSER_H
#define RDT_PROTOCOL_SERVER_PARSER_H


#include <string>
#include <vector>
#include "../packet_sender.h"

class server_parser {
private:
    std::vector<std::string> server_args;
public:
    explicit server_parser(std::string args_file);

    int get_port_number();

    int get_max_window_size();

    unsigned int get_random_seed();

    float get_loss_probability();

    std::string get_server_mode();

    loss_mode get_loss_mode();

    std::vector<int> get_loss_sequence();
};


#endif //RDT_PROTOCOL_SERVER_PARSER_H
