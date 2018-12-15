#ifndef RDT_PROTOCOL_PACKET_SENDER_H
#define RDT_PROTOCOL_PACKET_SENDER_H


#include <netinet/in.h>
#include <random>
#include <deque>
#include "../shared/data_packet.h"

enum loss_mode {
    PROBABILITY,
    WITH_SEQUENCE
};

class packet_sender {
public:
    static void set_mode(loss_mode mode);

    static void set_seed(unsigned int seed);

    static void set_probability(float loss_probability);

    static void send_packet(int server_socket, sockaddr_in client_socket, data_packet *packet);

    static void set_loss_sequence(std::vector<int> sequence);

private:
    static uint64_t packet_number;
    static int loss_sequence_index;
    static std::vector<int> loss_sequence;
    static loss_mode mode;
    static float loss_probability;
    static std::minstd_rand0 generator;
    static std::uniform_real_distribution<double> distribution;
};


#endif //RDT_PROTOCOL_PACKET_SENDER_H
