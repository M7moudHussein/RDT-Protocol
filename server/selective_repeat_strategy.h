//
// Created by mahmoud on 12/7/18.
//

#ifndef RDT_PROTOCOL_SELECTIVE_REPEAT_STRATEGY_H
#define RDT_PROTOCOL_SELECTIVE_REPEAT_STRATEGY_H

#include "rdt_strategy.h"

class selective_repeat_strategy : public rdt_strategy {
public:
    explicit selective_repeat_strategy(std::string file_name);

    explicit selective_repeat_strategy(std::string file_name, int window_size);

    void acknowledge_packet(ack_packet ack_pkt) override;

    void start() override;

    void advance_window() override;

private:
    int window_size;
    std::queue<data_packet *> window;

};


#endif //RDT_PROTOCOL_SELECTIVE_REPEAT_STRATEGY_H
