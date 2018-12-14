#include "packet_sender.h"
#include <functional>

float packet_sender::loss_probability;
std::minstd_rand0 packet_sender::generator;
std::uniform_real_distribution<double> packet_sender::distribution;
loss_mode packet_sender::mode;
int packet_sender::loss_sequence_index = 0;
int packet_sender::packet_number = 0;
std::vector<int> packet_sender::loss_sequence;

void packet_sender::set_mode(loss_mode mode) {
    packet_sender::mode = mode;
}

void packet_sender::set_seed(unsigned int seed) {
    packet_sender::generator = std::default_random_engine(seed);
    packet_sender::distribution = std::uniform_real_distribution<double>(0.0, 100.0);
}

void packet_sender::set_loss_sequence(std::vector<int> sequence) {
    packet_sender::loss_sequence = sequence;
}

void packet_sender::set_probability(const float loss_probability) {
    packet_sender::loss_probability = loss_probability;
}

void packet_sender::send_packet(int server_socket, sockaddr_in client_socket, data_packet *packet) {
    switch (packet_sender::mode) {
        case PROBABILITY:
            if (packet_sender::distribution(packet_sender::generator) > loss_probability) {
                sendto(server_socket, packet->pack().c_str(), packet->pack().length(),
                       MSG_CONFIRM, (const struct sockaddr *) &client_socket,
                       sizeof(client_socket));
            }
            break;
        case SEQUENTIAL:
            if (packet_number++ % packet_sender::loss_sequence[loss_sequence_index] == 0) {
                loss_sequence_index = loss_sequence_index++ % packet_sender::loss_sequence.size();
                sendto(server_socket, packet->pack().c_str(), packet->pack().length(),
                       MSG_CONFIRM, (const struct sockaddr *) &client_socket,
                       sizeof(client_socket));
            }
            break;
    }
}

