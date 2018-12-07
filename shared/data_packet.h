#ifndef RDT_PROTOCOL_DATA_GRAM_H
#define RDT_PROTOCOL_DATA_GRAM_H

#include <ctime>
#include <chrono>

#define DATAGRAM_SIZE
#define PACKET_SIZE 500

class data_packet {
public:
    data_packet(std::string data);

    uint16_t get_cksum() const;

    void set_cksum(uint16_t cksum);

    uint16_t get_len() const;

    void set_len(uint16_t len); // TODO: this should include header size

    uint32_t get_seqno() const;

    void set_seqno(uint32_t seqno);

    const std::string &get_data() const;

    std::chrono::steady_clock::time_point get_time_stamp() const;

    void ack();

    bool is_acked();

    void set_time_stamp(std::chrono::steady_clock::time_point time_stamp);

    friend std::ostream& operator <<(std::ostream &strm, const data_packet &packet);

    static struct time_comparator {
        bool operator()(const data_packet &lhs, const data_packet &rhs) {
            return lhs.get_time_stamp() < rhs.get_time_stamp();
        }
    };

private:
    uint16_t cksum;
    uint16_t len;
    uint32_t seqno;
    bool acked = false;
    std::chrono::steady_clock::time_point time_stamp;

    std::string data;
};


#endif //RDT_PROTOCOL_DATA_GRAM_H
