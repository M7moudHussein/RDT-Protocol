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
    // when ack received pop packet from window queue.

}

void selective_repeat_strategy::start() {
}

void selective_repeat_strategy::advance_window() {
    for(int i = 0; i < window_size && pkt_builder->has_next(); i++){
        window.push(pkt_builder->get_next_packet());
    }
}
