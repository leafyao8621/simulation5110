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
    this->input_size = 0;
}

uint32_t System::Machine::get_input_size() {
    return this->input_size;
}

void System::Machine::toggle_status() {
    this->is_down = !this->is_down;
}

bool System::Machine::load_input(System::Part part, uint64_t ts) {
    part.time_entered = ts;
    this->input_size++;
    if (this->input.empty()) {
        System::Batch bat(part.type);
        bat.cnt = 1;
        bat.parts.push(part);
        this->input.push(bat);
        return 1;
    } else {
        this->input.back().cnt++;
        this->input.back().parts.push(part);
        return 0;
    }
}

uint64_t System::Machine::load_machine(uint64_t ts) {
    if (!this->input.empty()) {
        this->input_size--;
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

void System::Machine::log(std::ostream& os) {
    os << (this->is_down ? "down\n" : "operational\n");
    if (this->is_busy) {
        os << "working on " << name[(uint32_t)(this->cur.type)] << '\n';
    } else {
        os << "idle\n";
    }
    // os << this->input_size << '\n';
    if (this->input_size) {
        os << "input size " << this->input_size << " top batch type " <<
        name[(uint32_t)(this->input.front().type)] << " size " <<
        this->input.front().cnt << '\n';
    } else {
        os << "no input\n";
    }
}
