#ifndef _STATS_H_
#define _STATS_H_

#include <cstdint>

class Stats {
    uint64_t num_produced[7];
    uint64_t num_orders_shipped[7];
public:
    Stats();
    void fulfil_order(uint32_t type);
    void ship_order(uint32_t type);
    void log(std::ostream& os);
};
#endif
