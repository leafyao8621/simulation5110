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

System::System(int32_t seed) {
    this->cur_time = 0;    
    System::Demand **iter = (System::Demand**)this->demand;
    for (char i = 0, **ii = name; i < 6; i++,
         *(iter++) =
         (System::Demand*)new System::DemandKDE(std::string("data/") +
                                                *(ii++) +
                                                std::string(".dmd"), seed));
    *iter = (System::Demand*)new System::DemandConst(0); 
    uint32_t init_backlog[6] = {58, 34, 15, 40, 14, 3};
    uint32_t *iter_init_backlog = init_backlog;
    uint32_t *iter_backlog = this->backlog;
    std::queue<System::Order> *iter_order =
    (std::queue<System::Order>*)this->order;
    for (int i = 0; i < 6; i++,
         (iter_order++)->push(Order((System::PartType)i,
                                    *iter_init_backlog, 0)),
         *(iter_backlog++) = *(iter_init_backlog++));
    *iter_backlog = 0;
    this->display_status(std::cout);
}

System::~System() {
    System::Demand **iter = (System::Demand**)this->demand;
    for (int i = 0; i < 7; delete *(iter++), i++);
}

void System::generate_order() {
    System::Demand **iter_dmd = (System::Demand**)this->demand;
    std::queue<System::Order> *iter_order =
    (std::queue<System::Order>*)this->order;
    uint32_t *iter_backlog = this->backlog;
    for (int i = 0; i < 7; i++, iter_dmd++, iter_order++, iter_backlog++) {
        uint32_t dmd = (**iter_dmd)();
        if (dmd) {
            iter_order->push(Order((System::PartType)i, dmd, this->cur_time));
            (*iter_backlog) += dmd;
        }
    }
}

bool System::fulfil_order(System::PartType type) {
    if (!this->order[(uint32_t)type].empty() &&
        this->order[(uint32_t)type].front().rem) {
        this->order[(uint32_t)type].front().rem--;
        this->backlog[(uint32_t)type]--;
        return 0;
    }
    return 1;
}

void System::ship_order(System::PartType type) {
    if (!this->order[(uint32_t)type].empty()) {
        this->order[(uint32_t)type].pop();
    }
}

// void System::enter_queue(System::Part part, uint32_t stage, uint32_t machine) {
//     std::cout << "not yet implemented\n";
// }

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
}

