#ifndef RDT_PROTOCOL_DATA_GRAM_H
#define RDT_PROTOCOL_DATA_GRAM_H

#define PACKET_SIZE 500

using std::string;

class datagram {
public:
    datagram();

    datagram(string data);

    uint16_t get_checksum();

    void set_checksum(uint16_t);

    uint16_t get_len();

    void set_len(uint16_t);

    uint32_t get_seq_no();

    void set_seq_no(uint32_t);

    string get_data();

    void build(string datagram_buffer);

    std::string to_string();

private:
    uint16_t checksum;
    uint16_t len;
    uint32_t seq_no;

    string data;
};


#endif