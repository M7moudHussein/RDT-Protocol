#ifndef RDT_PROTOCOL_DATA_GRAM_H
#define RDT_PROTOCOL_DATA_GRAM_H

#include <ctime>
#include <chrono>
#include "string"
#define PACKET_DATA_SIZE 500

class data_packet {
public:
    explicit data_packet(std::string data, uint32_t seqno);
    data_packet(char buffer[], int buf_len);

    uint16_t get_cksum() const;

    uint16_t get_len() const;

    uint32_t get_seqno() const;

    const std::string &get_data() const;

    std::string pack();

    void unpack(std::string);

    std::chrono::steady_clock::time_point get_time_stamp() const;

    void ack();

    bool is_acked();

    void set_time_stamp(std::chrono::steady_clock::time_point time_stamp);

    friend std::ostream &operator<<(std::ostream &strm, const data_packet &packet);

    bool operator< (const data_packet &right) const
    {
        return seqno < right.seqno;
    }

    struct time_comparator {
        bool operator()(const data_packet *lhs, const data_packet *rhs) const {
            return lhs->get_time_stamp() < rhs->get_time_stamp();
        }
    };

    struct seq_num_comparator {
        bool operator()(const data_packet &lhs, const data_packet &rhs) const {
            return lhs.get_seqno() < rhs.get_seqno();
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
