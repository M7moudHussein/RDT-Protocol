#include <utility>

#include "packet_builder.h"
#include "../../shared/packet_util.h"
#include <sys/stat.h>
#include <algorithm>
#include <zconf.h>
#include <iostream>

packet_builder::packet_builder(std::string relative_path, int queue_size) {
    struct stat statbuf{};
    std::string absolute_path = get_absolute_path(std::move(relative_path));
    if (stat(absolute_path.c_str(), &statbuf) == -1) {
        //TODO handle error
        exit(EXIT_FAILURE);
    }
    data_not_read = statbuf.st_size;
    std::cout << "remaining data size: " << data_not_read + packets_read_queue.size() << std::endl;
    buffer = new char[BUFFER_SIZE];

    fp = fopen(absolute_path.c_str(), "rb");

    if (fp == nullptr) {
        //TODO handle error
        exit(EXIT_FAILURE);
    }
    packet_builder::queue_size = queue_size;
}

data_packet *packet_builder::get_next_packet(uint32_t &next_seq_num) {
    if (data_not_read > 0 && packets_read_queue.empty()) {
        size_t read_length = fread(buffer, sizeof(char), BUFFER_SIZE, fp);
        data_not_read -= read_length;
        if (read_length == -1) {
            //TODO handle error
            exit(EXIT_FAILURE);
        }

        size_t packet_start_index = 0;
        while (read_length > 0) {
            std::string packet_data = std::string(buffer + packet_start_index,
                                                  std::min(read_length, (size_t) MAX_DATA_SIZE));

            packets_read_queue.push(new data_packet(packet_data, static_cast<uint32_t>(next_seq_num)));
            next_seq_num++;

            read_length -= packet_data.length();
            packet_start_index += packet_data.length();
        }
    }
    data_packet *next_packet = packets_read_queue.front();
    packets_read_queue.pop();
    if (!has_next()) {
        fclose(fp);
        delete[] buffer;
    }
    return next_packet;
}

bool packet_builder::has_next() {
    return data_not_read > 0 || !packets_read_queue.empty();
}

std::string packet_builder::get_absolute_path(std::string relative_path) {
    int const max_path_length = 200;
    char cwd[max_path_length];
    getcwd(cwd, sizeof(cwd));
    return std::string(cwd) + std::string("/") + relative_path;
}
