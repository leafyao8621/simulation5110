#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <cstdint>


#include "system.h"
#include "stats.h"

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
                                PriorityQueue* pq,
                                std::ostream& os) = 0;
        virtual void log(std::ostream& os) = 0;
        virtual ~Event() {}
    };
    class EventGenerateOrder : public Engine::Event {
    public:
        EventGenerateOrder(uint64_t ts) : Engine::Event(ts) {}
        virtual void operator()(System *model,
                                Stats *stats,
                                PriorityQueue* pq,
                                std::ostream& os);
        virtual void log(std::ostream& os);
    };
    class EventStartOrder : public Engine::Event {
    public:
        EventStartOrder(uint64_t ts) : Engine::Event(ts) {}
        virtual void operator()(System *model,
                                Stats *stats,
                                PriorityQueue* pq,
                                std::ostream& os);
        virtual void log(std::ostream& os);
    };
    class EventFulfilOrder : public Engine::Event {
        uint32_t type;
    public:
        EventFulfilOrder(uint64_t ts, uint32_t type);
        virtual void operator()(System *model,
                                Stats *stats,
                                PriorityQueue* pq,
                                std::ostream& os);
        virtual void log(std::ostream& os);
    };
    class EventShipOrder : public Engine::Event {
        uint32_t type;
    public:
        EventShipOrder(uint64_t ts, uint32_t type);
        virtual void operator()(System *model,
                                Stats *stats,
                                PriorityQueue* pq,
                                std::ostream& os);
        virtual void log(std::ostream& os);
    };
    class EventEnterQueue : public Engine::Event {
        uint32_t operation, machine;
        System::Part part;
    public:
        EventEnterQueue(uint64_t ts, System::Part part, uint32_t opration,
                        uint32_t machine);
        virtual void operator()(System *system,
                                Stats *stats,
                                PriorityQueue *pq,
                                std::ostream& os);
        virtual void log(std::ostream& os);
    };
    class EventEnterMachine : public Engine::Event {
        uint32_t operation, machine;
    public:
        EventEnterMachine(uint64_t ts, uint32_t operation, uint32_t machine);
        virtual void operator()(System *model,
                                Stats *stats,
                                PriorityQueue* pq,
                                std::ostream& os);
        virtual void log(std::ostream& os);
    };
    class EventEndDay : public Engine::Event {
    public:
        EventEndDay(uint64_t ts) : Engine::Event(ts) {}
        virtual void operator()(System *model,
                                Stats *stats,
                                PriorityQueue *pq,
                                std::ostream& os);
        virtual void log(std::ostream& os);
    };
    class EventStartDay : public Engine::Event {
    public:
        EventStartDay(uint64_t ts) : Engine::Event(ts) {}
        virtual void operator()(System *model,
                                Stats *stats,
                                PriorityQueue *pq,
                                std::ostream& os);
        virtual void log(std::ostream& os);
    };
    class EventEndWork : public Engine::Event {
        int32_t operation, machine;
    public:
        EventEndWork(uint64_t ts, uint32_t operation, uint32_t machine);
        virtual void operator()(System *model,
                                Stats *stats,
                                PriorityQueue *pq,
                                std::ostream& os);
        virtual void log(std::ostream& os);
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
    bool interim;
    std::ofstream os_backlog;
public:
    Engine(System *system, Stats *stats, uint64_t max_time,
           std::string backlog);
    void run(bool verbose, bool time, uint64_t interim, std::ostream& os);
    ~Engine();
};
#endif
