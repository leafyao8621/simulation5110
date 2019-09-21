#include <iostream>
#include <fstream>
#include <cstring>
#include "stats.h"

static char *name[7] = {
    (char*)"C17",
    (char*)"E26",
    (char*)"D20",
    (char*)"B15",
    (char*)"D25",
    (char*)"F35",
    (char*)"N99"
}; 

Stats::Stats(std::string config) {
    memset(this->num_orders_shipped, 0, 56);
    memset(this->num_produced, 0, 56);
    memset(this->sum_lead_time, 0, 56);
    {
        std::ifstream ifs_facility(config + "/facility.config");
        int num;
        std::vector<uint64_t> *iter_queue_length_ts =
        (std::vector<uint64_t>*)this->queue_length_ts;
        std::vector<uint64_t> *iter_queue_length_cur =
        (std::vector<uint64_t>*)this->queue_length_cur;
        std::vector<uint64_t> *iter_queue_length_sum =
        (std::vector<uint64_t>*)this->queue_length_sum;
        for (int i = 0; i < 4; i++, iter_queue_length_ts++,
                               iter_queue_length_cur++,
                               iter_queue_length_sum++) {
            ifs_facility >> num;
            for (int j = 0; j < num; j++,
                 iter_queue_length_cur->push_back(0),
                 iter_queue_length_ts->push_back(0),
                 iter_queue_length_sum->push_back(0));
        }
    }
}

void Stats::fulfil_order(uint32_t type) {
    this->num_produced[type]++;
}

void Stats::ship_order(uint32_t type, uint64_t lt) {
    this->num_orders_shipped[type]++;
    this->sum_lead_time[type] += lt;
}

void Stats::update_queue_length(uint64_t ts, uint32_t operation,
                                uint32_t machine, uint64_t ql) {
    if (ql != this->queue_length_cur[operation][machine]) {
        this->queue_length_sum[operation][machine] += 
        (ts - this->queue_length_ts[operation][machine]) * ql;
        this->queue_length_ts[operation][machine] = ts;
    }
}

void Stats::log(std::ostream& os, uint64_t ts) {
    for (uint64_t i = 0, *j = num_produced, *k = num_orders_shipped;
         i < 7; i++, j++, k++) {
        os << "part " << name[i] << " num produced " << *j <<
        " num order shipped " << *k << " avg lead time " <<
        this->sum_lead_time[i] / *j << " min\n";        
    }
    for (int i = 0; i < 4; i++) {
        os << "operation " << i << '\n';
        int k = 0;
        for (const auto& j : this->queue_length_sum[i]) {
            os << "machine " << k++ << " average queue length " <<
            j / ts << '\n';
        }
    }
}
