//
// Created by awalid on 12/8/18.
//

#include "client_go_back_N_strategy.h"
#include "../../shared/packet_util.h"

client_go_back_N_strategy::client_go_back_N_strategy(int wnd_size) {
    buffer = new char[MAX_PKT_SIZE];
    window_size = wnd_size;
    expected_seqno = 0;
    done = false;
}

void client_go_back_N_strategy::run() {

    struct sockaddr_in sender_address;
    socklen_t sender_addr_len = sizeof(sender_address);

    ssize_t bytes_received = recvfrom(client_socket, buffer, MAX_PKT_SIZE, MSG_WAITALL,
                                      (struct sockaddr *) &sender_address, &sender_addr_len);

    data_packet packet_received = data_packet(buffer, static_cast<int>(bytes_received));
    uint32_t seqno = packet_received.get_seqno();

    if (bytes_received > 0 && seqno >= expected_seqno && seqno < expected_seqno + window_size) { // in current window
        if (is_terminal_pkt(&packet_received)) {
            done = true;
        } else if (seqno == expected_seqno && packet_util::calculate_checksum(&packet_received) ==
                                              packet_received.get_cksum()) {// in order (this packet has a sequence number equal to the base of the receive window)
            deliver_packet(packet_received);
            send_ack(new ack_packet(packet_received.get_seqno())); // send ACK for received packet
        } else { // loss occurred so discard out of order packets received and send ack of last received packet
            uint32_t last_seqno = expected_seqno - 1;
            send_ack(new ack_packet(last_seqno));
        }
    }
}


void client_go_back_N_strategy::deliver_packet(data_packet pkt) {
    file_data.append(pkt.get_data());
    //write data to file immediately without buffering
    file_writer::write(file_data);
    file_data.clear();
    expected_seqno++;
}
