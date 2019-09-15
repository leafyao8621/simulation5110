#include "system.h"

System::Machine::Machine() {
    this->is_down = 0;
    this->is_busy = 0;
}

void System::Machine::toggle_status() {
    this->is_down = !this->is_down;
}

void System::Machine::load_input(System::Part part, uint64_t ts) {
    part.time_entered = ts;
    if (this->input.empty()) {
        System::Batch bat(part.type);
        bat.cnt = 1;
        bat.parts.push(part);
        this->input.push(bat);
    } else {
        this->input.back().cnt++;
        this->input.back().parts.push(part);
    }
}

uint64_t System::Machine::load_machine(uint64_t ts) {
    if (!this->input.empty()) {
        if (this->is_down) {
            return this->reopen_time;
        } 
        this->is_busy = 1;
        this->cur = this->input.front().parts.front();
        if (!(--(this->input.front().cnt))) {
            this->input.pop();
        }
    }
    return 0;
}
