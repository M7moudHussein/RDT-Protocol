//
// Created by mahmoud on 12/14/18.
//

#include "packet_sender.h"
#include <random>
#include <functional>

float packet_sender::loss_probability;
std::minstd_rand0 packet_sender::generator;
std::uniform_real_distribution<double> packet_sender::distribution;

void packet_sender::set_seed(unsigned int seed) {
    packet_sender::generator = std::default_random_engine(seed);
    packet_sender::distribution = std::uniform_real_distribution<double>(0.0, 100.0);
}

void packet_sender::set_probability(const float loss_probability) {
    packet_sender::loss_probability = loss_probability;
}

void packet_sender::send_packet(int server_socket, sockaddr_in client_socket, data_packet *packet) {
    if (packet_sender::distribution(packet_sender::generator) > loss_probability) {
        sendto(server_socket, packet->pack().c_str(), packet->pack().length(),
               MSG_CONFIRM, (const struct sockaddr *) &client_socket,
               sizeof(client_socket));
    }
}

