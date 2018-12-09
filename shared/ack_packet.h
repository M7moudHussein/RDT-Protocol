#ifndef RDT_PROTOCOL_ACK_PACKET_H
#define RDT_PROTOCOL_ACK_PACKET_H

#include <cstdint>
#include <ostream>
#include <chrono>

#define ACK_PKT_LEN 8

class ack_packet {
public:
    ack_packet();
    ack_packet(uint32_t ackno);

    uint16_t get_cksum() const;

    uint16_t get_len() const;

    uint32_t get_ackno() const;

    std::string pack();

    void unpack(std::string);

    friend std::ostream &operator<<(std::ostream &strm, const ack_packet &packet);

    std::chrono::steady_clock::time_point get_time_stamp() const;

    void set_time_stamp(std::chrono::steady_clock::time_point time_stamp);

private:
    uint16_t cksum, len;
    uint32_t ackno;

};


#endif
