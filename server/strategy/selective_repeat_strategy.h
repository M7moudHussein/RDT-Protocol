//
// Created by mahmoud on 12/7/18.
//

#ifndef RDT_PROTOCOL_SELECTIVE_REPEAT_STRATEGY_H
#define RDT_PROTOCOL_SELECTIVE_REPEAT_STRATEGY_H

#include "rdt_strategy.h"

class selective_repeat_strategy : public rdt_strategy {
public:
    explicit selective_repeat_strategy(std::string file_name, int max_window_size);

    void acknowledge_packet(ack_packet &ack_pkt) override;

    void start() override;

private:
    void send_packet(data_packet *packet) override;

    void handle_time_out() override;

    void advance_window() override;

    void expand_window();

    void shrink_window(int new_size);

    void adjust_window_size();
};


#endif //RDT_PROTOCOL_SELECTIVE_REPEAT_STRATEGY_H
