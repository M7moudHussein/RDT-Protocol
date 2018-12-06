#ifndef RDT_PROTOCOL_DATA_GRAM_H
#define RDT_PROTOCOL_DATA_GRAM_H

#define DATAGRAM_SIZE
#define PACKET_SIZE 500

class datagram {
private:
    uint16_t cksum;
    uint16_t len;
    uint32_t seqno;

    std::string data;
public:
    datagram(std::string data);
};


#endif //RDT_PROTOCOL_DATA_GRAM_H
