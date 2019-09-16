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
    System::PartType *iter_load_order =
    (System::PartType*)system->load_order;
    for (int i = 0; i < 7; i++, iter_load_order++) {
        if (!system->order[(uint32_t)(*iter_load_order)].empty()) {
            uint32_t operation = i == 5 ? 3 : 0;
            uint32_t amt =
            system->order[(uint32_t)(*iter_load_order)].front().amt;
            uint64_t dst = system->routing[(uint32_t)(*iter_load_order)][0];
            uint32_t dst_num = (dst &
                            0xff00000000000000) >> 56;
            uint32_t machine = dst & 0xff;
            uint32_t msk = 0xff00;
            uint32_t input_size =
            system->facility[operation][machine].get_input_size();
            for (int j = 1; j < dst_num; j++, msk <<= 8) {
                uint32_t tmp = (dst & msk) >> (j << 3);
                uint32_t tmp_size =
                system->facility[operation][tmp].get_input_size();
                if (tmp_size < input_size) {
                    machine = tmp;
                    input_size = tmp_size;
                }
            }
            for (int j = 0; j < amt; j++) {
                if (system->facility[0][machine]
                        .load_input(System::Part(*iter_load_order,
                                    system->priority[(uint32_t)(*iter_load_order)]),
                                    system->cur_time)) {
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

void Engine::EventEnterMachine::operator()(System* system,
                                           Stats* stats,
                                           PriorityQueue* pq) {
    system->enter_machine(this->operation, this->machine);
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
    system->end_work(this->operation, this->machine);
    pq->push(new EventEnterMachine(this->ts + 10, operation, machine));
}