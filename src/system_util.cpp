#include <iostream>
#include "system.h"

static char *name[7] = {
    (char*)"C17",
    (char*)"E26",
    (char*)"D20",
    (char*)"B15",
    (char*)"D25",
    (char*)"F35",
    (char*)"N99"
}; 

void System::display_status(std::ostream& os) {
    os << "time " << this->cur_time / 10080 << " weeks " <<
    (cur_time % 10080) / 1440 << " days " << (cur_time % 10080 % 1440) / 60 <<
    ':' << cur_time % 10080 % 1440 % 60 << "\ncurrent backlog\n";
    std::queue<System::Order> *iter_order =
    (std::queue<System::Order>*)this->order;
    uint32_t *iter_backlog = this->backlog;
    char** iter_name = name;
    for (int i = 0; i < 7; i++,
         os << "part " << *(iter_name++) << " total remaining " <<
         *(iter_backlog++) << " num orders " << iter_order->size() <<
         " current order remaining " << iter_order->front().rem <<
         '\n', iter_order++);
    os << "machines\n";
    for (int i = 0; i < 4; i++) {
        os << "operation " << i << '\n';
        for (int j = 0; j < this->facility[i].size(); j++) {
            os << "machine " << j << '\n';
            this->facility[i][j].log(os);
        }
    }
}

void System::display_config(std::ostream& os) {
    os << "priority\n";
    uint32_t *iter_priority = this->priority;
    for (int i = 0; i < 7;
         os << "part " << name[(uint32_t)(i++)] << " priority " <<
         *(iter_priority++) << '\n');
    os << "load order\n";
    System::PartType *iter_load_order = this->load_order;
    os << name[(uint32_t)(*(iter_load_order++))];
    for (int i = 1; i < 7; i++,
         os << " -> " << name[(uint32_t)(*(iter_load_order++))]);
    os << "\nfacility\n";
    std::vector<System::Machine> *iter_facility =
    (std::vector<System::Machine>*)this->facility;
    for (int i = 0; i < 4;
         os << "operation " << i++ << " num machines " <<
         (iter_facility++)->size() << '\n');
    os << "time table\n";
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 7; j++) {
            os << "operation " << i << " part " << name[j] <<
            " setup time " << this->changeover_time[i][j] <<
            " mins process time " << this->process_time[i][j] << " mins\n"; 
        }
    }
    os << "routing table:\n";
    uint64_t num, msk;
    for (int i = 0; i < 7; i++) {
        os << "part " << name[i] << ' ';
        for (int j = 0; j < 4; j++) {
            num = (this->routing[i][j] & 0xff00000000000000) >> 56;
            if (num) {
                msk = 0xff;
                os << (this->routing[i][j] & msk);
                msk <<= 8;
                for (int k = 1; k < num;
                     os << " | " << ((this->routing[i][j] & msk) >> (k << 3)),
                     k++);
                
            } else {
                os << 'X';
            }
            os << " -> ";
        }
        os << "out\n";
    }
}
