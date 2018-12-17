#include <cstdint>
#include "data_packet.h"
#include "ack_packet.h"

#ifndef RDT_PROTOCOL_PACKET_UTIL_H
#define RDT_PROTOCOL_PACKET_UTIL_H

#define HEADER_SIZE 8
#define MAX_DATA_SIZE 500
#define MAX_PKT_SIZE 508

namespace packet_util {
    const std::chrono::seconds PACKET_TIME_OUT = std::chrono::seconds(1);

    uint16_t calculate_checksum(data_packet *packet);
    uint16_t calculate_checksum(ack_packet *packet);
};


#endif
