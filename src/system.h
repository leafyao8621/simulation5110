#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <string>
#include <vector>
#include <queue>
#include <cstdint>

#include "MRG32K3a.h"

class System {
public:
    enum PartType {
        C17,
        E26,
        D20,
        B15,
        D25,
        F35,
        N99
    };
    struct Part {
        uint32_t priority;
        uint64_t time_entered, time_started, time_stopped;
        System::PartType type;
    public:
        Part(System::PartType type, uint32_t priority);
        Part();
    };
    struct Order {
        System::PartType type;
        uint32_t amt, rem;
        uint64_t time_ordered;
    public:
        Order(System::PartType type, uint32_t amt, uint64_t time_ordered);
    };
    class PartComp {
    public:
        bool operator()(System::Part a, System::Part b);
    };
    using PriorityQueue =
    std::priority_queue<System::Part, std::vector<System::Part>, PartComp>;
    class Machine {
        bool is_down, is_busy;
        uint64_t reopen_time;
        System::Part cur;
        PriorityQueue input;
    public:
        Machine();
        uint32_t get_input_size();
        bool get_is_busy();
        bool get_is_down();
        uint64_t get_reopen_time();
        System::Part get_cur();
        System::Part get_top_queue();
        void shut_down(uint64_t cur_time, uint64_t reopen_time);
        void turn_on();
        bool load_input(System::Part part, uint64_t ts);
        uint64_t load_machine(uint64_t ts);
        System::Part remove_part();
        void log(std::ostream& os);
    };
    class Demand {
    public:
        MRG32K3a generator;
        virtual ~Demand() {}
        virtual uint32_t operator()() = 0;
    };
    class DemandKDE : private System::Demand {
        int32_t data[100];
        double h, mean, c;
    public:
        DemandKDE(std::string fn, int seed);
        virtual uint32_t operator()();
    };
    class DemandConst : private System::Demand {
        uint32_t val;
    public:
        DemandConst(uint32_t val);
        virtual uint32_t operator()();
    };
private:
    std::vector<System::Machine> facility[4];
    uint64_t routing[7][4];
    std::queue<System::Part> buffer;
    std::queue<System::Order> order[7];
    uint32_t backlog[7];
    uint32_t priority[7];
    System::PartType load_order[7];
    System::Demand *demand[7];
    uint64_t changeover_time[4][7];
    uint64_t process_time[4][7];
public:
    uint64_t cur_time;
    System(int32_t seed, std::string config);
    System::PartType get_load_order(uint32_t ind);
    uint64_t get_process_time(uint32_t operation, uint32_t machine);
    uint64_t get_changeover_time(uint32_t operation, System::PartType type);
    System::Part get_part(uint32_t operation, uint32_t machine);
    System::Part get_top_queue(uint32_t operation, uint32_t machine);
    bool get_order_empty(System::PartType type);
    uint32_t get_last_order_amt(System::PartType type);
    uint32_t get_routing(System::PartType type, uint32_t operation);
    uint32_t get_input_size(uint32_t operation, uint32_t machine);
    uint32_t get_priority(System::PartType type);
    void generate_order();
    bool fulfil_order(System::PartType type);
    uint64_t ship_order(System::PartType type);
    bool enter_input(Part part, uint32_t operation, uint32_t machine,
                     uint64_t& ql);
    uint64_t enter_machine(uint32_t operation, uint32_t machine, uint64_t& ql);
    void end_day();
    void start_day();
    uint64_t end_work(uint32_t operation, uint32_t machine);
    void display_status(std::ostream& os);
    void display_config(std::ostream& os);
    ~System();
};
#endif
