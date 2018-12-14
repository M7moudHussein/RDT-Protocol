#ifndef RDT_PROTOCOL_PACKET_SENDER_H
#define RDT_PROTOCOL_PACKET_SENDER_H


#include <netinet/in.h>
#include <random>
#include "../shared/data_packet.h"

class packet_sender {
public:
    static void set_seed(unsigned int seed);

    static void set_probability(float loss_probability);

    static void send_packet(int server_socket, sockaddr_in client_socket, data_packet *packet);

private:
    static float loss_probability;
    static std::minstd_rand0 generator;
    static std::uniform_real_distribution<double> distribution;
};


#endif //RDT_PROTOCOL_PACKET_SENDER_H
