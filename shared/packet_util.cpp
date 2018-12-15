#include <cstdint>
#include <iostream>
#include "data_packet.h"
#include "ack_packet.h"
#include "packet_util.h"

void wrap_extra_bits(uint32_t *check_sum) {
    if ((*check_sum) & 0x10000) {
        *check_sum = ((*check_sum) & 0xFFFF) + 1;
    }
}

uint16_t packet_util::calculate_checksum(data_packet *packet) {
    uint32_t check_sum = packet->get_len();
    wrap_extra_bits(&check_sum);

    check_sum = check_sum + (packet->get_seqno() & 0xFFFF);
    wrap_extra_bits(&check_sum);

    check_sum = check_sum + ((packet->get_seqno() >> 16) & 0xFFFF);
    wrap_extra_bits(&check_sum);

    for (char i : packet->get_data()) {
        check_sum += ((uint32_t) i) & 0xFF;
        wrap_extra_bits(&check_sum);
    }

    if (check_sum > 0xFFFF) {
        perror("Error calculating data packet checksum");
        exit(EXIT_FAILURE);
    }

    return static_cast<uint16_t>(check_sum);
}

uint16_t packet_util::calculate_checksum(ack_packet *packet) {
    uint32_t check_sum = packet->get_len();
    wrap_extra_bits(&check_sum);

    check_sum = check_sum + (packet->get_ackno() & 0xFFFF);
    wrap_extra_bits(&check_sum);

    check_sum = check_sum + ((packet->get_ackno() >> 16) & 0xFFFF);
    wrap_extra_bits(&check_sum);

    if (check_sum > 0xFFFF) {
        perror("Error calculating data packet checksum");
        exit(EXIT_FAILURE);
    }

    return static_cast<uint16_t>(check_sum);
}
