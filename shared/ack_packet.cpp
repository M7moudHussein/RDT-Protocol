#include "ack_packet.h"

std::ostream &operator<<(std::ostream &strm, const ack_packet &packet) {
    strm << packet.get_cksum();
    strm << packet.get_len();
    strm << packet.get_ackno();
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

std::string ack_packet::pack() {
    char *buf = new char[len];

    buf[0] = ackno & 0xFF;
    buf[1] = (ackno >> 8) & 0xFF;
    buf[2] = (ackno >> 16) & 0xFF;
    buf[3] = (ackno >> 24) & 0xFF;

    buf[4] = len & 0xFF;
    buf[5] = (len >> 8) & 0xFF;

    buf[6] = cksum & 0xFF;
    buf[7] = (cksum >> 8) & 0xFF;

    std::string pkt = std::string(buf, len);
    delete[]buf;
    return pkt;
}

void ack_packet::unpack(std::string buf) {
    uint32_t u0 = buf[0], u1 = buf[1], u2 = buf[2], u3 = buf[3];
    ackno = u0 | (u1 << 8) | (u2 << 16) | (u3 << 24);

    uint16_t x0 = buf[4], x1 = buf[5];
    len = x0 | (x1 << 8);

    uint16_t y0 = buf[6], y1 = buf[7];
    cksum = y0 | (y1 << 8);
}

