#include <utility>

#include "packet_sender.h"
#include <functional>
#include <iostream>

float packet_sender::loss_probability;
std::minstd_rand0 packet_sender::generator;
std::uniform_real_distribution<double> packet_sender::distribution;
int packet_sender::loss_sequence_index = 0;
uint64_t packet_sender::packet_number = 1;
std::vector<int> packet_sender::loss_sequence;

void packet_sender::set_seed(unsigned int seed) {
    packet_sender::generator = std::default_random_engine(seed);
    packet_sender::distribution = std::uniform_real_distribution<double>(0.0, 100.0);
}

void packet_sender::set_loss_sequence(std::vector<int> sequence) {
    packet_sender::loss_sequence = std::move(sequence);
}

void packet_sender::set_probability(const float loss_probability) {
    packet_sender::loss_probability = loss_probability * 100;
}

bool packet_sender::send_packet(int server_socket, sockaddr_in client_address, data_packet *packet) {
    if (packet_number++ % packet_sender::loss_sequence[loss_sequence_index] != 0) {
        if (packet_sender::distribution(packet_sender::generator) > loss_probability) {
            sendto(server_socket, packet->pack().c_str(), packet->pack().length(),
                   MSG_CONFIRM, (const struct sockaddr *) &client_address,
                   sizeof(client_address));
        } else {
            std::cout << "Packet with seq no " << packet->get_seqno() << " was lost!" << std::endl;
        }
        return false;
    } else {
        // Sequence has passed, three duplicate acks detected
        sendto(server_socket, packet->pack().c_str(), packet->pack().length(),
               MSG_CONFIRM, (const struct sockaddr *) &client_address,
               sizeof(client_address));
        packet_number = 1;
        loss_sequence_index++;
        loss_sequence_index = loss_sequence_index % (int32_t) packet_sender::loss_sequence.size();
        return true;
    }
}

