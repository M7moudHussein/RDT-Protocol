//
// Created by mahmoud on 12/8/18.
//

#ifndef RDT_PROTOCOL_GO_BACK_N_STRATEGY_H
#define RDT_PROTOCOL_GO_BACK_N_STRATEGY_H


#include "rdt_strategy.h"

class go_back_N_strategy : public rdt_strategy {
public:
    go_back_N_strategy(std::string file_name);

    void acknowledge_packet(ack_packet &ack_pkt) override;

    void start() override;

private:
    void send_packet(data_packet *packet) override;
    void advance_window() override;
    void handle_time_out() override;
};


#endif //RDT_PROTOCOL_GO_BACK_N_STRATEGY_H
