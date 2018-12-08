#include <cstdint>
#include "data_packet.h"
#include "ack_packet.h"

#ifndef RDT_PROTOCOL_PACKET_UTIL_H
#define RDT_PROTOCOL_PACKET_UTIL_H

#define HEADER_SIZE 8

void wrap_extra_bits(int *check_sum);

uint16_t calculate_checksum(data_packet packet);

uint16_t calculate_checksum(ack_packet packet);

void wrap_extra_bits(int *check_sum);

#endif
