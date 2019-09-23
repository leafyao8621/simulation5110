#include <iostream>
#include "engine.h"
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

Engine::Event::Event(uint64_t ts) {
    this->ts = ts;
}

bool Engine::EventComp::operator()(Engine::Event* a, Engine::Event* b) {
    return a->ts > b->ts;
}

void Engine::EventGenerateOrder::log(std::ostream& os) {
    os << "time " << this->ts / 10080 << " weeks " <<
    (this->ts % 10080) / 1440 << " days " << (this->ts % 10080 % 1440) / 60 <<
    ':' << this->ts % 10080 % 1440 % 60 << " generate order\n";
}

void Engine::EventGenerateOrder::operator()(System* system,
                                            Stats* stats,
                                            PriorityQueue* pq,
                                            std::ostream& os) {
    system->generate_order();
    pq->push(new Engine::EventStartOrder(this->ts + 10080));
    pq->push(new Engine::EventGenerateOrder(this->ts + 10080));
}

void Engine::EventStartOrder::operator()(System* system,
                                         Stats* stats,
                                         PriorityQueue* pq,
                                         std::ostream& os) {
    for (int i = 0; i < 7; i++) {
        if (!system->get_order_empty(system->get_load_order(i))) {
            uint32_t operation = i == 5 ? 3 : 0;
            uint32_t amt =
            system->get_last_order_amt((System::PartType)i);
            uint64_t dst = system->get_routing((System::PartType)(i), operation);
            uint32_t dst_num = (dst &
                                0xff00000000000000) >> 56;
            uint32_t machine = dst & 0xff;
            uint32_t msk = 0xff00;
            uint32_t input_size =
            system->get_input_size(operation, machine);
            for (int j = 1; j < dst_num; j++, msk <<= 8) {
                uint32_t tmp = (dst & msk) >> (j << 3);
                uint32_t tmp_size =
                system->get_input_size(operation, tmp);
                if (tmp_size < input_size) {
                    machine = tmp;
                    input_size = tmp_size;
                }
            }
            uint64_t dummy;
            for (int j = 0; j < amt; j++) {
                if (system->enter_input(System::Part((System::PartType)i,
                    system->get_priority((System::PartType)(i)) + this->ts),
                    operation, machine, dummy)) {
                    pq->push(new EventEnterMachine(this->ts + 10,
                                                   operation, machine));
                }
            }
        }
    }
}

void Engine::EventStartOrder::log(std::ostream& os) {
    os << "time " << this->ts / 10080 << " weeks " <<
    (this->ts % 10080) / 1440 << " days " << (this->ts % 10080 % 1440) / 60 <<
    ':' << this->ts % 10080 % 1440 % 60 << " start order\n";
}

Engine::EventShipOrder::EventShipOrder(uint64_t ts,
                                       uint32_t type) :
Engine::Event(ts) {
    this->type = type;
}

void Engine::EventShipOrder::operator()(System* system,
                                        Stats* stats,
                                        PriorityQueue* pq,
                                        std::ostream& os) {
    uint64_t lt = system->ship_order((System::PartType)this->type);
    stats->ship_order(this->type, lt);
}

void Engine::EventShipOrder::log(std::ostream& os) {
    os << "time " << this->ts / 10080 << " weeks " <<
    (this->ts % 10080) / 1440 << " days " << (this->ts % 10080 % 1440) / 60 <<
    ':' << this->ts % 10080 % 1440 % 60 << " ship order " <<
    name[this->type] << '\n';
}

Engine::EventFulfilOrder::EventFulfilOrder(uint64_t ts,
                                           uint32_t type) :
Engine::Event(ts) {
    this->type = type;
}

void Engine::EventFulfilOrder::operator()(System* system,
                                          Stats* stats,
                                          PriorityQueue* pq,
                                          std::ostream& os) {
    bool real;
    if (system->fulfil_order((System::PartType)this->type, real)) {
        pq->push((new Engine::EventShipOrder(this->ts, this->type)));
    }
    if (real) {
        stats->fulfil_order(this->type);
    }
}

void Engine::EventFulfilOrder::log(std::ostream& os) {
    os << "time " << this->ts / 10080 << " weeks " <<
    (this->ts % 10080) / 1440 << " days " << (this->ts % 10080 % 1440) / 60 <<
    ':' << this->ts % 10080 % 1440 % 60 << " fulfil order " <<
    name[this->type] << '\n';
}

Engine::EventEnterMachine::EventEnterMachine(uint64_t ts,
                                             uint32_t operation,
                                             uint32_t machine) : 
Engine::Event(ts) {
    this->operation = operation;
    this->machine = machine;
}

Engine::EventEnterQueue::EventEnterQueue(uint64_t ts, System::Part part,
                                         uint32_t operation, uint32_t machine) :
Engine::Event(ts) {
    this->part = part;
    this->operation = operation;
    this->machine = machine;
}

void Engine::EventEnterQueue::operator()(System *system,
                                         Stats *stats,
                                         PriorityQueue *pq,
                                         std::ostream& os) {
    uint64_t ql;
    if (system->enter_input(this->part, this->operation, this->machine, ql)) {
        pq->push(new EventEnterMachine(this->ts + 10,
                                       this->operation, this->machine));
        stats->update_queue_length(this->ts, this->operation,
                                   this->machine, ql);
    }
}

void Engine::EventEnterQueue::log(std::ostream& os) {
    os << "time " << this->ts / 10080 << " weeks " <<
    (this->ts % 10080) / 1440 << " days " << (this->ts % 10080 % 1440) / 60 <<
    ':' << this->ts % 10080 % 1440 % 60 << " enter queue operation " <<
    this->operation << " machine " << this->machine << " part " <<
    name[(uint32_t)this->part.type] << '\n';
}

void Engine::EventEnterMachine::operator()(System* system,
                                           Stats* stats,
                                           PriorityQueue* pq,
                                           std::ostream& os) {
    uint64_t ql;
    uint64_t res = system->enter_machine(this->operation, this->machine, ql);
    if (res) {
        pq->push(new EventEnterMachine(res, this->operation, this->machine));
    } else {
        pq->push(new EventEndWork(this->ts +
                                  system->get_process_time(this->operation,
                                  this->machine),
                                  this->operation, this->machine));
        stats->update_queue_length(this->ts, this->operation,
                                   this->machine, ql);
    }
}

void Engine::EventEnterMachine::log(std::ostream& os) {
    os << "time " << this->ts / 10080 << " weeks " <<
    (this->ts % 10080) / 1440 << " days " << (this->ts % 10080 % 1440) / 60 <<
    ':' << this->ts % 10080 % 1440 % 60 << " enter machine operation " <<
    this->operation << " machine " << this->machine << '\n';
}

void Engine::EventEndDay::operator()(System *system,
                                     Stats* stats,
                                     PriorityQueue *pq,
                                     std::ostream& os) {
    system->end_day();
    system->display_backlog(os);
    if ((this->ts % 10080) / 1440 != 4) {
        pq->push((new Engine::EventStartDay(this->ts + 480)));
    } else {
        pq->push((new Engine::EventStartDay(this->ts + 3360)));
    }
}

void Engine::EventEndDay::log(std::ostream& os) {
    os << "time " << this->ts / 10080 << " weeks " <<
    (this->ts % 10080) / 1440 << " days " << (this->ts % 10080 % 1440) / 60 <<
    ':' << this->ts % 10080 % 1440 % 60 << " end day\n";
}

void Engine::EventStartDay::operator()(System *system,
                                       Stats* stats,
                                       PriorityQueue *pq,
                                       std::ostream& os) {
    system->start_day();
    pq->push((new Engine::EventEndDay(this->ts + 960)));
}

void Engine::EventStartDay::log(std::ostream& os) {
    os << "time " << this->ts / 10080 << " weeks " <<
    (this->ts % 10080) / 1440 << " days " << (this->ts % 10080 % 1440) / 60 <<
    ':' << this->ts % 10080 % 1440 % 60 << " start day\n";
}

Engine::EventEndWork::EventEndWork(uint64_t ts,
                                   uint32_t operation,
                                   uint32_t machine) :
Engine::Event(ts) {
    this->operation = operation;
    this->machine = machine;
}

void Engine::EventEndWork::operator()(System *system,
                                      Stats *stats,
                                      PriorityQueue *pq,
                                      std::ostream& os) {
    System::Part part = system->get_part(this->operation, this->machine);
    uint64_t res = system->end_work(this->operation, this->machine);
    if (res & 0x4000000000000000) {
        pq->push(new EventFulfilOrder(this->ts, res & 0xbfffffffffffffff));
        if (system->get_part(this->operation, this->machine).type ==
            system->get_top_queue(this->operation, this->machine).type) {
            pq->push(new EventEnterMachine(this->ts + 10, this->operation,
                                           this->machine));
        } else {
            pq->push(new EventEnterMachine(this->ts +
            system->get_changeover_time(this->operation,
            system->get_top_queue(this->operation, this->machine).type),
            this->operation,
            this->machine));
        }
    } else if (res & 0x8000000000000000) {
        pq->push(new EventEndWork((res & 0x7fffffffffffffff) +
                                  system->get_process_time(this->operation,
                                                           this->machine) -
                                  (part.time_stopped - part.time_started),
                                  this->operation, this->machine));
    } else {
        pq->push(new EventEnterQueue(this->ts, part, this->operation + 1,
                                       res & 0x7fffffffffffffff));
        if (system->get_part(this->operation, this->machine).type ==
            system->get_top_queue(this->operation, this->machine).type) {
            pq->push(new EventEnterMachine(this->ts + 10, this->operation,
                                           this->machine));
        } else {
            pq->push(new EventEnterMachine(this->ts +
            system->get_changeover_time(this->operation,
            system->get_top_queue(this->operation, this->machine).type),
            this->operation, this->machine));
        }
    }
}

void Engine::EventEndWork::log(std::ostream& os) {
    os << "time " << this->ts / 10080 << " weeks " <<
    (this->ts % 10080) / 1440 << " days " << (this->ts % 10080 % 1440) / 60 <<
    ':' << this->ts % 10080 % 1440 % 60 << " end work operation " <<
    this->operation << " machine " << this->machine << '\n';
}
