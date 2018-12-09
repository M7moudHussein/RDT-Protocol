#ifndef RDT_PROTOCOL_PACKET_BUILDER_H
#define RDT_PROTOCOL_PACKET_BUILDER_H


#include <string>
#include <queue>
#include "../../shared/data_packet.h"

class packet_builder {
public:
    packet_builder(std::string absolute_path, int queue_size);

    data_packet *get_next_packet(int &next_seq_num);

    bool has_next();

private:
    std::string get_absolute_path(std::string relative_path);

    FILE *fp;
    int queue_size;
    size_t data_not_read;
    std::queue<data_packet *> packets_read_queue;
    char *buffer;

#define BUFFER_SIZE PACKET_SIZE * queue_size
};


#endif //RDT_PROTOCOL_PACKET_BUILDER_H
