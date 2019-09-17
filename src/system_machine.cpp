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

System::Machine::Machine() {
    this->is_down = 0;
    this->is_busy = 0;
}

uint32_t System::Machine::get_input_size() {
    return this->input.size();
}

bool System::Machine::get_is_busy() {
    return this->is_busy;
}

bool System::Machine::get_is_down() {
    return this->is_down;
}

uint64_t System::Machine::get_reopen_time() {
    return this->reopen_time;
}

System::Part System::Machine::get_cur() {
    return this->cur;
}

void System::Machine::shut_down(uint64_t cur_time, uint64_t reopen_time) {
    this->is_down = 1;
    this->reopen_time = reopen_time;
    if (this->is_busy) {
        this->cur.time_stopped = cur_time;
    }
}

void System::Machine::turn_on() {
    this->is_down = 0;
}

bool System::Machine::load_input(System::Part part, uint64_t ts) {
    part.time_entered = ts;
    bool out = this->input.empty();
    this->input.push(part);
    return out;
}

uint64_t System::Machine::load_machine(uint64_t ts) {
    if (!this->input.empty()) {
        if (this->is_down) {
            return this->reopen_time;
        } 
        this->is_busy = 1;
        this->cur = this->input.top();
        this->cur.time_started = ts;
        this->input.pop();
    }
    return 0;
}

System::Part System::Machine::remove_part() {
    this->is_busy = 0;
    return this->cur;
}

void System::Machine::log(std::ostream& os) {
    os << (this->is_down ? "down\n" : "operational\n");
    if (this->is_busy) {
        os << "working on " << name[(uint32_t)(this->cur.type)] << '\n';
    } else {
        os << "idle\n";
    }
    // os << this->input_size << '\n';
    if (!this->input.empty()) {
        os << "queue length " << this->input.size() << " top item " <<
        name[(uint32_t)(this->input.top().type)] << '\n';
    } else {
        os << "no queue\n";
    }
}
