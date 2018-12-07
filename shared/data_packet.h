#ifndef RDT_PROTOCOL_DATA_GRAM_H
#define RDT_PROTOCOL_DATA_GRAM_H

#define DATAGRAM_SIZE
#define PACKET_SIZE 500

class data_packet {
public:
    data_packet(std::string data);

    uint16_t get_cksum() const;

    uint16_t get_len() const;

    uint32_t get_seqno() const;

    const std::string &get_data() const;

    friend std::ostream& operator <<(std::ostream &strm, const data_packet &packet);

private:
    uint16_t cksum;
    uint16_t len;
    uint32_t seqno;

    std::string data;
};


#endif //RDT_PROTOCOL_DATA_GRAM_H
