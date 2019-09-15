#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <queue>
#include <vector>
#include <cstdint>

class System;
class Stats;

class Engine {
public:
    class Event;
    class EventComp;
    using PriorityQueue =
    std::priority_queue<Event*, std::vector<Event*>, EventComp>;
public:
    class Event {
    public:
        uint64_t ts;
        Event(uint64_t ts);
        virtual void operator()(System *model,
                                Stats *stats,
                                PriorityQueue* pq) = 0;
    };
    class EventGenerateOrder : public Engine::Event {
    public:
        EventGenerateOrder(uint64_t ts) : Engine::Event(ts) {}
        virtual void operator()(System *model,
                                Stats *stats,
                                PriorityQueue* pq);
    };
    class EventFulfilOrder : public Engine::Event {
        uint32_t type;
    public:
        EventFulfilOrder(uint64_t ts, uint32_t type);
        virtual void operator()(System *model,
                                Stats *stats,
                                PriorityQueue* pq);
    };
    class EventShipOrder : public Engine::Event {
        uint32_t type;
    public:
        EventShipOrder(uint64_t ts, uint32_t type);
        virtual void operator()(System *model,
                                Stats *stats,
                                PriorityQueue* pq);
    };
public:
    class EventComp {
    public:
        bool operator()(Engine::Event* a, Engine::Event* b);
    };
private:
    PriorityQueue event_queue;
    System *system;
    Stats *stats;
    uint64_t max_time;
public:
    Engine(System *system, Stats *stats, uint64_t max_time);
    void run();
    ~Engine();
};
#endif
