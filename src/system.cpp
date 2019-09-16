#include <iostream>
#include <fstream>
#include <algorithm>
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

System::System(int32_t seed, std::string(config)) {
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
    {
        std::ifstream ifs_changeover(std::string("data/changeover.time"));
        std::ifstream ifs_process(std::string("data/process.time"));
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 7; j++) {
                ifs_changeover >> this->changeover_time[i][j];
                ifs_process >> this->process_time[i][j];
            }
        }
    }
    {
        std::ifstream ifs_route(config + "/route.config");
        uint64_t num, tmp;
        for (int i = 0; i < 7; i++) {
            for (int j = 0; j < 4; j++) {
                ifs_route >> num;
                this->routing[i][j] = num << 56;
                for (int k = 0; k < num; ifs_route >> tmp,
                        this->routing[i][j] |= tmp << (k << 3), k++);
            }
        }
    }
    {
        std::ifstream ifs_facility(config + "/facility.config");
        int num;
        std::vector<System::Machine> *iter_facility =
        (std::vector<System::Machine>*)this->facility;
        for (int i = 0; i < 4; i++, iter_facility++) {
            ifs_facility >> num;
            for (int j = 0; j < num; j++,
                 iter_facility->push_back(System::Machine()));
        }
    }
    {
        std::ifstream ifs_priority(config + "/priority.config");
        uint32_t *iter_priority = this->priority;
        System::PartType *iter_load_order = this->load_order;
        for (int i = 0; i < 7;
             ifs_priority >> *(iter_priority++),
             *(iter_load_order++) = (System::PartType)(i++));
    }
    std::sort(this->load_order, this->load_order + 1,
              [&] (System::PartType a, System::PartType b) {
        return this->priority[a] > this->priority[b];
    });
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

void System::enter_machine(uint32_t operation, uint32_t machine) {
    this->facility[operation][machine].load_machine(this->cur_time);
}

void System::end_day() {
    std::vector<System::Machine> *iter_facility =
    (std::vector<System::Machine>*)this->facility;
    for (int i = 0; i < 4; i++, iter_facility++) {
        for (auto& m : *(iter_facility)) {
            m.shut_down();
        }
    }
}

void System::start_day() {
    std::vector<System::Machine> *iter_facility =
    (std::vector<System::Machine>*)this->facility;
    for (int i = 0; i < 4; i++, iter_facility++) {
        for (auto& m : *(iter_facility)) {
            m.turn_on();
        }
    }
}
