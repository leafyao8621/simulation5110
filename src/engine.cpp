#include <iostream>
#include <queue>
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

Engine::Engine(System *system, Stats *stats, uint64_t max_time,
               std::string backlog) {
    this->system = system;
    this->stats = stats;
    this->max_time = max_time;
    this->event_queue.push((Engine::Event*)new Engine::EventGenerateOrder(0));
    this->event_queue.push((Engine::Event*)new Engine::EventEndDay(960));
    this->interim = 1;
    this->os_backlog.open(backlog);
}

void Engine::run(bool verbose, bool time, uint64_t interim, std::ostream& os) {
    os << "initial state\n";
    this->os_backlog << *name;
    for (char i = 1, **j = name + 1; i < 7; i++, this->os_backlog << ',' << *(j++));
    this->os_backlog << '\n';
    this->system->display_status(std::cout);
    for (; this->system->cur_time < max_time && !this->event_queue.empty();
         this->event_queue.pop()) {
        this->system->cur_time = this->event_queue.top()->ts;
        if (time) {
            os << this->system->cur_time << ' ';
            (*(this->event_queue.top())).log(os);
        }
        (*(this->event_queue.top()))(this->system, this->stats,
                                     &(this->event_queue), this->os_backlog);
        delete this->event_queue.top();
        if (this->interim && interim <= this->system->cur_time) {
            this->system->display_status(os);
            this->interim = 0;
        }
        if (verbose) {
            this->system->display_status(os);
        }
    }
}

Engine::~Engine() {
    for (; !this->event_queue.empty(); delete this->event_queue.top(),
                                       this->event_queue.pop());
    this->os_backlog.close();
}
