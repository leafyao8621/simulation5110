#include <iostream>
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

Stats::Stats() {
    memset(this->num_orders_shipped, 0, 56);
    memset(this->num_produced, 0, 56);
}

void Stats::fulfil_order(uint32_t type) {
    this->num_produced[type]++;
}

void Stats::ship_order(uint32_t type) {
    this->num_orders_shipped[type]++;
}

void Stats::log(std::ostream& os) {
    for (uint64_t i = 0, *j = num_produced, *k = num_orders_shipped;
         i < 7; i++, j++, k++) {
        os << "part " << name[i] << " num produced " << *j <<
        " num order shipped " << *k << '\n';        
    }
}
