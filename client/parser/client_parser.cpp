#include <fstream>
#include "client_parser.h"

client_parser::client_parser(string args_file_path) {
    std::ifstream args_file(args_file_path);
    args_file >> server_ip;
    args_file >> server_port_no;
    args_file >> local_port_no;
    args_file >> req_file_name;
    args_file >> default_window_size;
}

string client_parser::get_server_ip() {
    return server_ip;
}

uint16_t client_parser::get_server_port_no() {
    return server_port_no;
}

string client_parser::get_req_file_name() {
    return req_file_name;
}

uint16_t client_parser::get_local_port_no() {
    return local_port_no;
}

int client_parser::get_default_window_size() {
    return default_window_size;
}
