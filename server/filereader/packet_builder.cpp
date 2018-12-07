#include "packet_builder.h"
#include <sys/stat.h>
#include <algorithm>

packet_builder::packet_builder(std::string absolute_path, int queue_size) {
    struct stat statbuf;
    if (stat(absolute_path.c_str(), &statbuf) == -1) {
        //TODO handle error
    }
    data_not_read = statbuf.st_size;
    buffer = new char[BUFFER_SIZE];

    FILE *fp = fopen(absolute_path.c_str(), "rb");

    if (fp == NULL) {
        //TODO handle error
    }
}

data_packet *packet_builder::get_next_packet() {
    if (data_not_read > 0 && packets_read_queue.empty()) {
        size_t read_length = fread(buffer, sizeof(char), BUFFER_SIZE, fp);
        data_not_read -= read_length;

        if (data_not_read <= 0) {
            fclose(fp);
            delete[] buffer;
            delete fp;
        } else if (read_length == -1) {
            //TODO handle error
        }

        size_t packet_start_index = 0;
        while (read_length > 0) {
            std::string packet_data = std::string(buffer + packet_start_index,
                                                  std::min(read_length, (size_t) PACKET_SIZE));

            packets_read_queue.push(new data_packet(packet_data));

            read_length -= PACKET_SIZE;
            packet_start_index += PACKET_SIZE;
        }
    }
    data_packet *next_packet = packets_read_queue.front();
    packets_read_queue.pop();
    return next_packet;
}

bool packet_builder::has_next() {
    return data_not_read > 0;
}
