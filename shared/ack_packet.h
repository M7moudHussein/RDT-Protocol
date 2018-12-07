//
// Created by awalid on 12/7/18.
//

#ifndef RDT_PROTOCOL_ACK_PACKET_H
#define RDT_PROTOCOL_ACK_PACKET_H

#include <cstdint>
#include <ostream>

class ack_packet {
public:
    ack_packet();

    uint16_t get_cksum() const;

    uint16_t get_len() const;

    uint32_t get_ackno() const;

    friend std::ostream &operator<<(std::ostream &strm, const ack_packet &packet);

private:
    uint16_t cksum, len;
    uint32_t ackno;
};


#endif //RDT_PROTOCOL_ACK_PACKET_H
