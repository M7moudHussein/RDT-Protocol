#include <iostream>
#include "client_selective_repeat_strategy.h"
#include "../../shared/packet_util.h"

client_selective_repeat_strategy::client_selective_repeat_strategy(int wnd_size) {
    window_size = wnd_size;
    expected_seqno = 0;
    done = false;
}

void client_selective_repeat_strategy::run() {
    struct sockaddr_in sender_address;
    socklen_t sender_addr_len = sizeof(sender_address);
    char *buffer = new char[MAX_PKT_SIZE];

    ssize_t bytes_received = recvfrom(client_socket, buffer, MAX_PKT_SIZE, MSG_WAITALL,
                                      (struct sockaddr *) &sender_address, &sender_addr_len);

    data_packet packet_received = data_packet(buffer, static_cast<int>(bytes_received));
    uint32_t seqno = packet_received.get_seqno();

    if (seqno >= expected_seqno && seqno < expected_seqno + window_size) { // in current window
        if (is_terminal_pkt(&packet_received)) {
            done = true; // TODO: check, since this flag terminates the algorithm, client will not be informed if the corresponding ACK is lost, server may keep waiting for the ACK
        } else if (window.find(packet_received) == window.end() && packet_util::calculate_checksum(&packet_received) == packet_received.get_cksum()) { // if the packet was not previously received
            window.insert(packet_received); // buffer
            if (seqno == expected_seqno) // in order (this packet has a sequence number equal to the base of the receive window)
                deliver_buffered_packets();
        }
        send_ack(new ack_packet(packet_received.get_seqno())); // send ACK for received packet
    } else if (seqno >= expected_seqno - window_size && seqno < expected_seqno) { // in the previous window
        send_ack(new ack_packet(packet_received.get_seqno())); // packet already received and ACKed -> resend ACK for it
    }
}

void client_selective_repeat_strategy::deliver_buffered_packets() {
    /* This packet, and any previously buffered and CONSECUTIVELY numbered
     * (beginning with rcv_base [expected_seqno]) packets are delivered to the upper layer. */
    for (auto pkt_iter = window.begin(); pkt_iter != window.end();) {
        if ((*pkt_iter).get_seqno() == expected_seqno) { // consecutive
            file_data.append((*pkt_iter).get_data());
            window.erase(pkt_iter); // remove buffered packet
            file_writer::write(file_data); // write buffered data to file on intervals and then clear file data buffer to be used later again.
            file_data.clear();
            expected_seqno++;
        }
    }
}
