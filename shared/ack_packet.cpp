//
// Created by awalid on 12/7/18.
//

#include "ack_packet.h"

std::ostream &operator <<(std::ostream &strm, const ack_packet &packet) {
    strm << packet.get_cksum() << "\n";
    strm << packet.get_len() << "\n";
    strm << packet.get_ackno() << "\n";
    strm << std::flush;
    return strm;
}

ack_packet::ack_packet() {
    cksum = len = 0;
    ackno = 0;
}

uint16_t ack_packet::get_cksum() const {
    return cksum;
}

uint16_t ack_packet::get_len() const {
    return len;
}

uint32_t ack_packet::get_ackno() const {
    return ackno;
}
