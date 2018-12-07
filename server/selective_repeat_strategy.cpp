#include <utility>

//
// Created by mahmoud on 12/7/18.
//

#include "selective_repeat_strategy.h"

#define DEFAULT_WINDOW_SIZE 200 //TODO just random window size... it should be changed to the right value

selective_repeat_strategy::selective_repeat_strategy(std::string file_name, int window_size) {
    this->pkt_builder = new packet_builder(std::move(file_name), window_size);
}

selective_repeat_strategy::selective_repeat_strategy(std::string file_name) {
    window_size = DEFAULT_WINDOW_SIZE;
    this->pkt_builder = new packet_builder(std::move(file_name), window_size);
}

void selective_repeat_strategy::acknowledge_packet(ack_packet ack_pkt) {
    // when ack received pop packet from window queue.

    //after ack advance window. if first one is acked otherwise find acked packet and set it to true ( Acked )


//    window.pop();
//    if(pkt_builder->has_next())
//        window.push(pkt_builder->get_next_packet());
}

void selective_repeat_strategy::start() {
    //fill queue of unacked packets with N packets
    fill_window();
    while (!window.empty()) {
        // do the sending logic

        if (initial_pkt) {
            //initialize timer thread then sleep it till timeout of 1st unacked packet
            initial_pkt = false;
            timer = new timer_thread(new std::thread(&selective_repeat_strategy::resend_packet));
        }
        timer->sleep_until(std::chrono::steady_clock::now() + PACKET_TIME_OUT);
    }


}

void selective_repeat_strategy::fill_window() {
    for (int i = 0; i < window_size && pkt_builder->has_next(); i++) {
        window.push(pkt_builder->get_next_packet());
    }
}

void selective_repeat_strategy::resend_packet() {

}
