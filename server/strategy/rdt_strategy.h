#ifndef RDT_PROTOCOL_RDT_STRATEGY_H
#define RDT_PROTOCOL_RDT_STRATEGY_H

#include <netinet/in.h>
#include <thread>
#include <sstream>
#include <fstream>
#include "../../shared/ack_packet.h"
#include "../filereader/packet_builder.h"
#include "../timer_thread.h"

class rdt_strategy {
public:
    virtual void acknowledge_packet(ack_packet &ack_pkt) = 0;

    virtual void start() = 0;

    void set_client_address(sockaddr_in client_address) {
        this->client_address = client_address;
    }

    void set_server_socket(int server_socket) {
        this->server_socket = server_socket;
    }

    bool is_done() {
        if (window.empty()) {
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            auto time_spent = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_time_stamp).count();

            std::ofstream stats_file;
            stats_file.open("stats.txt");
            for (auto wz: window_size_history)
                stats_file << wz << std::endl;
            stats_file << "Packets sent: " << packets_sent << std::endl;
            stats_file << "Time spent: " << time_spent << "msec" << std::endl;
            stats_file.close();
        }
        return this->window.empty();
    }

    void send_empty_packet() {
        // Assuming here no loss in sending this empty packet
        auto packet = new data_packet(std::string(), next_seq_number);
        sendto(server_socket, packet->pack().c_str(), packet->pack().length(),
               0, (const struct sockaddr *) &client_address,
               sizeof client_address);
    }


protected:
    std::vector<int> window_size_history;
    uint32_t packets_sent = 0;
    std::chrono::steady_clock::time_point start_time_stamp;

    void fill_window() {
        while (window.size() < window_size && pkt_builder->has_next()) {
            window.push_back(pkt_builder->get_next_packet(next_seq_number));
        }
    }

    virtual void advance_window() = 0;

    virtual void handle_time_out() = 0;

    virtual void send_packet(data_packet *packet) = 0;

    packet_builder *pkt_builder;
    sockaddr_in client_address;
    int server_socket;
    timer_thread *timer;
    std::set<data_packet *, data_packet::time_comparator> unacked_packets;
    std::mutex set_mutex;
    std::mutex wnd_mutex;
    std::deque<data_packet *> window;
    std::set<data_packet *, data_packet::ptr_seq_num_comparator> aux_window;
    uint32_t next_seq_number;
    int window_size;
    int max_window_size;
    int threshold;
};

#endif //RDT_PROTOCOL_RDT_STRATEGY_H
