#include "system.h"

System::Part::Part(System::PartType type, uint32_t priority) {
    this->type = type;
    this->priority = priority;
}

System::Part::Part() {
    this->time_entered = 0;
    this->type = System::PartType::C17;
}

System::Batch::Batch(System::PartType type) {
    this->type = type;
    this->cnt = 0;
}

System::Batch::Batch() {
    this->type = System::PartType::C17;
    this->cnt = 0;
}

System::Order::Order(System::PartType type,
                     uint32_t amt,
                     uint64_t time_ordered) {
    this->type = type;
    this->amt = this->rem = amt;
    this->time_ordered = time_ordered;
}

bool System::PartComp::operator()(System::Part a, System::Part b) {
    return a.priority < b.priority;
}
