//
// Created by awalid on 12/8/18.
//

#ifndef RDT_PROTOCOL_CLIENT_GO_BACK_N_STRATEGY_H
#define RDT_PROTOCOL_CLIENT_GO_BACK_N_STRATEGY_H


#include "client_rdt_strategy.h"

class client_go_back_N_strategy : public client_rdt_strategy {
private:
    void deliver_packet(data_packet pkt);

public:
    explicit client_go_back_N_strategy(int window_size);
    void run() override;
};


#endif //RDT_PROTOCOL_CLIENT_GO_BACK_N_STRATEGY_H
