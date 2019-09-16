#include <iostream>
#include <queue>
#include "engine.h"
#include "system.h"

Engine::Engine(System *system, Stats *stats, uint64_t max_time) {
    this->system = system;
    this->stats = stats;
    this->max_time = max_time;
    this->event_queue.push((Engine::Event*)new Engine::EventGenerateOrder(0));
    this->event_queue.push((Engine::Event*)new Engine::EventEndDay(960));
}

void Engine::run() {
    std::cout << "initial state\n";
    this->system->display_status(std::cout);
    for (; this->system->cur_time < max_time && !this->event_queue.empty(); this->event_queue.pop()) {
        (*(this->event_queue.top()))(this->system, this->stats,
                                     &(this->event_queue));
        this->system->cur_time = this->event_queue.top()->ts;
        delete this->event_queue.top();
        this->system->display_status(std::cout);
    }
}
Engine::~Engine() {
    for (; !this->event_queue.empty(); delete this->event_queue.top(),
                                       this->event_queue.pop());
}
