//
// Created by awalid on 12/8/18.
//

#ifndef RDT_PROTOCOL_PACKET_PARSER_H
#define RDT_PROTOCOL_PACKET_PARSER_H

#include <vector>
#include "string"
#include "data_packet.h"

class packet_parser {
public:
    packet_parser();
    std::vector<data_packet> parse(char *buffer);
private:
    std::string rest;
};


#endif //RDT_PROTOCOL_PACKET_PARSER_H
