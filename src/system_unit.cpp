#include "system.h"

System::Part::Part(uint64_t time_entered, System::PartType type) {
    this->time_entered = time_entered;
    this->type = type;
}

System::Part::Part() {
    this->time_entered = 0;
    this->type = System::PartType::C17;
}
System::Batch::Batch(System::PartType type) {
    this->type = type;
}

System::Batch::Batch() {
    this->type = System::PartType::C17;
}

System::Order::Order(System::PartType type,
                     uint32_t amt,
                     uint64_t time_ordered) {
    this->type = type;
    this->amt = this->rem = amt;
    this->time_ordered = time_ordered;
}
