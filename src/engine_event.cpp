#include <iostream>
#include "engine.h"
#include "system.h"

Engine::Event::Event(uint64_t ts) {
    this->ts = ts;
}

bool Engine::EventComp::operator()(Engine::Event* a, Engine::Event* b) {
    return a->ts > b->ts;
}

void Engine::EventGenerateOrder::operator()(System* system,
                                            Stats* stats,
                                            PriorityQueue* pq) {
    system->generate_order();
    pq->push(new Engine::EventStartOrder(this->ts + 10080));
    pq->push(new Engine::EventGenerateOrder(this->ts + 10080));
}

void Engine::EventStartOrder::operator()(System* system,
                                         Stats* stats,
                                         PriorityQueue* pq) {
    for (int i = 0; i < 7; i++) {
        if (!system->get_order_empty(system->get_load_order(i))) {
            uint32_t operation = i == 5 ? 3 : 0;
            uint32_t amt =
            system->get_top_order_amt((System::PartType)i);
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
            for (int j = 0; j < amt; j++) {
                if (system->enter_input(System::Part((System::PartType)i,
                    system->get_priority((System::PartType)(i))),
                    operation, machine)) {
                    pq->push(new EventEnterMachine(this->ts + 10,
                                                   operation, machine));
                }
            }
        }
    }
}

Engine::EventShipOrder::EventShipOrder(uint64_t ts,
                                       uint32_t type) :
Engine::Event(ts) {
    this->type = type;
}

void Engine::EventShipOrder::operator()(System* system,
                                        Stats* stats,
                                        PriorityQueue* pq) {
    system->ship_order((System::PartType)this->type);
}

Engine::EventFulfilOrder::EventFulfilOrder(uint64_t ts,
                                           uint32_t type) :
Engine::Event(ts) {
    this->type = type;
}

void Engine::EventFulfilOrder::operator()(System* system,
                                          Stats* stats,
                                          PriorityQueue* pq) {
    if (system->fulfil_order((System::PartType)this->type)) {
        pq->push((new Engine::EventShipOrder(this->ts, this->type)));
    }
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
                                         PriorityQueue *pq) {
    if (system->enter_input(this->part, this->operation, this->machine)) {
        pq->push(new EventEnterMachine(this->ts + 10, this->operation, this->machine));
    }
}
void Engine::EventEnterMachine::operator()(System* system,
                                           Stats* stats,
                                           PriorityQueue* pq) {
    uint64_t res = system->enter_machine(this->operation, this->machine);
    if (res) {
        pq->push(new EventEnterMachine(res, this->operation, this->machine));
    } else {
        pq->push(new EventEndWork(this->ts +
                                  system->get_process_time(this->operation,
                                  this->machine),
                                  this->operation, this->machine));
    }
}

void Engine::EventEndDay::operator()(System *system,
                                     Stats* stats,
                                     PriorityQueue *pq) {
    system->end_day();
    pq->push((new Engine::EventStartDay(this->ts + 480)));
}

void Engine::EventStartDay::operator()(System *system,
                                       Stats* stats,
                                       PriorityQueue *pq) {
    system->start_day();
    pq->push((new Engine::EventEndDay(this->ts + 960)));
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
                                      PriorityQueue *pq) {
    System::Part part = system->get_part(this->operation, this->machine);
    uint64_t res = system->end_work(this->operation, this->machine);
    if (res & 0x4000000000000000) {
        std::cout << "shit\n";
        pq->push(new EventFulfilOrder(this->ts, res & 0xbfffffffffffffff));
    } else if (res & 0x8000000000000000) {
        pq->push(new EventEndWork((res & 0x7fffffffffffffff) +
                                  system->get_process_time(this->operation,
                                                           this->machine) -
                                  (part.time_stopped - part.time_started),
                                  this->operation, this->machine));
    } else {
        pq->push(new EventEnterQueue(this->ts, part, this->operation + 1,
                                       res & 0x7fffffffffffffff));
        pq->push(new EventEnterMachine(this->ts, this->operation,
                                       this->machine));
    }
}