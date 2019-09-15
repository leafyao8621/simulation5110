#include <iostream>
#include "engine.h"
#include "system.h"

Engine::Engine(System *system, Stats *stats, uint64_t max_time) {
    this->system = system;
    this->stats = stats;
    this->max_time = max_time;
    this->event_queue.push((Engine::Event*)new Engine::EventGenerateOrder(0));
}

void Engine::run() {
    for (; this->system->cur_time < max_time && !this->event_queue.empty(); this->event_queue.pop()) {
        (*(this->event_queue.top()))(this->system, this->stats,
                                     &(this->event_queue));
        this->system->cur_time = this->event_queue.top()->ts;
        delete this->event_queue.top();
    }
}
Engine::~Engine() {
    for (; !this->event_queue.empty(); delete this->event_queue.top(),
                                       this->event_queue.pop());
}
