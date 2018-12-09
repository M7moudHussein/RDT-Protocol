#include <string>
#include <iostream>
#include <cstring>
#include <utility>
#include "data_packet.h"
#include "packet_util.h"

data_packet::data_packet(std::string data, uint32_t seqno) {
    data_packet::data = std::move(data);
    data_packet::len = static_cast<uint16_t>(data_packet::data.length() + HEADER_SIZE);
    data_packet::seqno = seqno;
    data_packet::cksum = packet_util::calculate_checksum(this);
}

data_packet::data_packet(char buffer[], int buf_len) {
    data_packet::unpack(std::string(buffer, static_cast<unsigned long>(buf_len)));
}

std::ostream &operator<<(std::ostream &strm, const data_packet &packet) {
    strm << packet.get_cksum() << "\n";
    strm << packet.get_len() << "\n";
    strm << packet.get_seqno() << "\n";
    strm << packet.get_data();
    strm << std::flush;
    return strm;
}

uint16_t data_packet::get_cksum() const {
    return cksum;
}

uint16_t data_packet::get_len() const {
    return len;
}

uint32_t data_packet::get_seqno() const {
    return seqno;
}

const std::string &data_packet::get_data() const {
    return data;
}

std::string data_packet::pack() {
    char buf[len];

    buf[0] = seqno & 0xFF;
    buf[1] = (seqno >> 8) & 0xFF;
    buf[2] = (seqno >> 16) & 0xFF;
    buf[3] = (seqno >> 24) & 0xFF;

    buf[4] = len & 0xFF;
    buf[5] = (len >> 8) & 0xFF;

    buf[6] = cksum & 0xFF;
    buf[7] = (cksum >> 8) & 0xFF;

    for (int i = 0, j = 8; i < data.length(); i++, j++)
        buf[j] = data[i];

    std::string pkt = std::string(buf, len);
    return pkt;
}

void data_packet::unpack(std::string buf) {
    uint32_t u0 = buf[0], u1 = buf[1], u2 = buf[2], u3 = buf[3];
    seqno = u0 | (u1 << 8) | (u2 << 16) | (u3 << 24);

    uint16_t x0 = buf[4], x1 = buf[5];
    len = x0 | (x1 << 8);

    uint16_t y0 = buf[6], y1 = buf[7];
    cksum = y0 | (y1 << 8);
    data = buf.substr(8, len - HEADER_SIZE);
}

std::chrono::steady_clock::time_point data_packet::get_time_stamp() const {
    return this->time_stamp;
}

void data_packet::set_time_stamp(std::chrono::steady_clock::time_point time_stamp) {
    this->time_stamp = time_stamp;
}

void data_packet::ack() {
    acked = true;
}

bool data_packet::is_acked() {
    return acked;
}
