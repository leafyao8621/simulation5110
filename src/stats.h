#ifndef _STATS_H_
#define _STATS_H_

#include <cstdint>
#include <vector>
#include <string>

class Stats {
    uint64_t num_produced[7];
    uint64_t num_orders_shipped[7];
    double sum_lead_time[7];
    std::vector<uint64_t> queue_length_ts[4];
    std::vector<uint64_t> queue_length_cur[4];
    std::vector<double> queue_length_sum[4];
public:
    Stats(std::string fn);
    void fulfil_order(uint32_t type);
    void ship_order(uint32_t type, uint64_t lt);
    void log(std::ostream& os, uint64_t ts);
    void update_queue_length(uint64_t ts, uint32_t operation, uint32_t machine,
                             uint64_t ql);
};
#endif
