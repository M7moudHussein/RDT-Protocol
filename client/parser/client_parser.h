#ifndef RDT_PROTOCOL_CLIENT_PARSER_H
#define RDT_PROTOCOL_CLIENT_PARSER_H

#include <string>

using std::string;

class client_parser {
public:
    client_parser(string args_file_path);

    string get_server_ip();

    uint16_t get_server_port_no();

    string get_req_file_name();

    uint16_t get_local_port_no();

    int get_default_window_size();

private:
    /* Server Properties */
    string server_ip;
    uint16_t server_port_no;

    /* Client Properties */
    uint16_t local_port_no;
    string req_file_name;
    int default_window_size;
};


#endif
