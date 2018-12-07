//
// Created by mahmoud on 12/7/18.
//

#include "selective_repeat_strategy.h"

#define DEFAULT_WINDOW_SIZE 200 //TODO just random window size... it should be changed to the right value

selective_repeat_strategy::selective_repeat_strategy(std::string file_name, int window_size) {
    this->pkt_builder = new packet_builder(file_name, window_size);
}

selective_repeat_strategy::selective_repeat_strategy(std::string file_name) {
    window_size = DEFAULT_WINDOW_SIZE;
    this->pkt_builder = new packet_builder(file_name, window_size);
}

void selective_repeat_strategy::acknowledge_packet(ack_packet ack_pkt) {

}

void selective_repeat_strategy::start() {

}
