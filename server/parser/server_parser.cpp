//
// Created by awalid on 12/5/18.
//

#include <fstream>
#include "server_parser.h"

server_parser::server_parser(std::string args_file) {
    std::ifstream input_file_stream(args_file);
    std::string line;
    while (std::getline(input_file_stream, line))
        server_parser::server_args.push_back(line);
}

int server_parser::get_port_number() {
    return stoi(server_parser::server_args[0]);
}

int server_parser::get_max_window_size() {
    return stoi(server_parser::server_args[1]);
}

unsigned int server_parser::get_random_seed() {
    return static_cast<unsigned int>(stoul(server_parser::server_args[2]));
}

float server_parser::get_loss_probability() {
    return stof(server_parser::server_args[3]);
}

std::string server_parser::get_server_mode() {
    return server_parser::server_args[4];
}


