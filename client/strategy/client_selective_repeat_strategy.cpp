#include <iostream>
#include "client_selective_repeat_strategy.h"
#include "../../shared/packet_util.h"

client_selective_repeat_strategy::client_selective_repeat_strategy(int window_size) {
    client_selective_repeat_strategy::window_size = window_size;
    client_selective_repeat_strategy::expected_seqno = 0;
}

void client_selective_repeat_strategy::run() {
    // Start receiving data packets from server
    struct sockaddr_in sender_address;
    socklen_t sender_addr_len = sizeof(sender_address);
    char *buffer = new char[MAX_PKT_SIZE];
    ssize_t bytes_received = recvfrom(client_socket, buffer, MAX_PKT_SIZE,
                              MSG_WAITALL, (struct sockaddr *) &sender_address,
                              &sender_addr_len);
    std::cout << bytes_received << std::endl;
    data_packet packet_received = data_packet(buffer, static_cast<int>(bytes_received));
    std::cout << packet_received << std::endl;
    std::cout << "Exiting..." << std::endl;
    exit(EXIT_SUCCESS);
    // TODO: Chunking to receive data from socket
    // Assume buffer now has only 1 data packet

    // Construct packet from buffer
    for (const data_packet &curr_received_pkt : parser.parse(buffer)) {
        int seqno = curr_received_pkt.get_seqno();
        if (seqno >= expected_seqno && seqno < expected_seqno + window_size) {
            // In window
            if (seqno != expected_seqno) {
                // Out of order
                window.insert(curr_received_pkt);
            } else {
                // In order
                for (const data_packet &window_pkt : window) {
                    if (window_pkt.get_seqno() == expected_seqno) {
                        file_data.append(window_pkt.get_data());
                        expected_seqno++;
                    }
                }
            }
            // send ack for received packet
            send_ack(new ack_packet(curr_received_pkt.get_seqno()));
        } else if (seqno < expected_seqno) {
            // Packet already written in file, send ack for it
            send_ack(new ack_packet(curr_received_pkt.get_seqno()));
        }
    }
}
