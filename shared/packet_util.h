#include <cstdint>
#include "data_packet.h"
#include "ack_packet.h"

#ifndef RDT_PROTOCOL_DATAGRAM_UTIL_H
#define RDT_PROTOCOL_DATAGRAM_UTIL_H

#endif

#define HEADER_SIZE 8

void wrap_extra_bits(int *check_sum);

uint16_t calculate_checksum(data_packet packet) {
    int check_sum = packet.get_len();
    wrap_extra_bits(&check_sum);

    check_sum = check_sum + packet.get_seqno();
    wrap_extra_bits(&check_sum);

    for (char i : packet.get_data()) {
        check_sum += i;
        wrap_extra_bits(&check_sum);
    }

    if (check_sum > 0xFFFF) {
        perror("Error calculating data packet checksum");
        exit(EXIT_FAILURE);
    }

    return static_cast<uint16_t>(check_sum);
}

uint16_t calculate_checksum(ack_packet packet) {
    int check_sum = packet.get_len();
    wrap_extra_bits(&check_sum);

    check_sum = check_sum + packet.get_ackno();
    wrap_extra_bits(&check_sum);

    if (check_sum > 0xFFFF) {
        perror("Error calculating ack packet checksum");
        exit(EXIT_FAILURE);
    }

    return static_cast<uint16_t>(check_sum);
}

void wrap_extra_bits(int *check_sum) {
    if (*check_sum & 0x10000) {
        *check_sum = (*check_sum & 0x0FFFF) + 1;
    }
}