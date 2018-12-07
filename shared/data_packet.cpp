#include <string>
#include <iostream>
#include "data_packet.h"

data_packet::data_packet(std::string data) {
    this->data = data;
}

std::ostream &operator <<(std::ostream &strm, const data_packet &packet) {
    strm << packet.get_seqno();
    strm << packet.get_len() << packet.get_cksum();
    strm << packet.get_data() << std::flush;
    return strm;
}

std::istream &operator>>(std::istream &strm, data_packet &packet) {
    std::string pkt_stream;
    strm >> pkt_stream;
    packet.set_seqno(stoi(pkt_stream.substr(0, 16)));
    return strm;
}

uint16_t data_packet::get_cksum() const {
    return cksum;
}

void data_packet::set_cksum(uint16_t cksum) {
    this->cksum = cksum;
}

uint16_t data_packet::get_len() const {
    return len;
}

void data_packet::set_len(uint16_t len) {
    this->len = len;
}

uint32_t data_packet::get_seqno() const {
    return seqno;
}

void data_packet::set_seqno(uint32_t seqno) {
    this->seqno = seqno;
}

const std::string &data_packet::get_data() const {
    return data;
}
