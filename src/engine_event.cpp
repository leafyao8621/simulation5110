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
    system->start_order();
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
