//
// Created by salma on 12/17/18.
//

#ifndef RDT_PROTOCOL_STOP_AND_WAIT_STRATEGY_H
#define RDT_PROTOCOL_STOP_AND_WAIT_STRATEGY_H


#include "rdt_strategy.h"

class stop_and_wait_strategy : public rdt_strategy {
public:

    explicit stop_and_wait_strategy(std::string file_name, int window_size, int max_window_size);

    void acknowledge_packet(ack_packet &ack_pkt) override;

    void start() override;

private:
    void send_packet(data_packet *packet) override;

    void handle_time_out() override;

    void advance_window() override;
};


#endif //RDT_PROTOCOL_STOP_AND_WAIT_STRATEGY_H
