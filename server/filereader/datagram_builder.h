#ifndef RDT_PROTOCOL_DATAGRAM_BUILDER_H
#define RDT_PROTOCOL_DATAGRAM_BUILDER_H


#include <string>
#include <queue>
#include "../../shared/data_packet.h"

class datagram_builder {
public:
    datagram_builder(std::string absolute_path, int queue_size);

    data_packet *get_next_datagram();

    bool has_next();

private:
    FILE *fp;
    int queue_size;
    size_t data_not_read;
    std::queue<data_packet *> datagram_queue;
    char *buffer;

#define BUFFER_SIZE PACKET_SIZE * queue_size
};


#endif //RDT_PROTOCOL_DATAGRAM_BUILDER_H
