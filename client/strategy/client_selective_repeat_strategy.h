#ifndef RDT_PROTOCOL_CLIENT_SELECTIVE_REPEAT_STRATEGY_H
#define RDT_PROTOCOL_CLIENT_SELECTIVE_REPEAT_STRATEGY_H


#include "client_rdt_strategy.h"

class client_selective_repeat_strategy : public client_rdt_strategy {
private:

public:
    explicit client_selective_repeat_strategy(int window_size);
    void run() override;
};


#endif
