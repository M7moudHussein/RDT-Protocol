#include <string>
#include <sstream>
#include "datagram.h"

datagram::datagram() {

}

datagram::datagram(std::string data) {
    this->data = data;
}

uint16_t datagram::get_checksum() {
    return 0;
}

void datagram::set_checksum(uint16_t checksum) {
    this->checksum = checksum;
}

uint16_t datagram::get_len() {
    return 0;
}

void datagram::set_len(uint16_t len) {
    this->len = len;
}

uint32_t datagram::get_seq_no() {
    return 0;
}

void datagram::set_seq_no(uint32_t seq_no) {
    this->seq_no = seq_no;
}

string datagram::get_data() {
    return std::__cxx11::string();
}

void datagram::build(string datagram_buffer) {
    std::stringstream buffer_stream(datagram_buffer);
    string cur_line;

    getline(buffer_stream, cur_line);
    checksum = static_cast<uint16_t>(stoi(cur_line));

    getline(buffer_stream, cur_line);
    len = static_cast<uint16_t>(stoi(cur_line));

    getline(buffer_stream, cur_line);
    seq_no = static_cast<uint32_t>(stoi(cur_line));

    while (getline(buffer_stream, cur_line))
        data += cur_line;
}

string datagram::to_string() {
    string datagram_buffer;
    datagram_buffer += checksum + "\n";
    datagram_buffer += len + "\n";
    datagram_buffer += seq_no + "\n";
    datagram_buffer += data;
    return datagram_buffer;
}
