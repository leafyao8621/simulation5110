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
        uint64_t time_entered, time_started, time_stopped;
        System::PartType type;
    public:
        Part(uint64_t time_entered, System::PartType type);
        Part();
    };
    struct Batch {
        System::PartType type;
        uint32_t cnt;
        std::queue<System::Part> parts;
    public:
        Batch(System::PartType type);
        Batch();
    };
    struct Order {
        System::PartType type;
        uint32_t amt, rem;
        uint64_t time_ordered;
    public:
        Order(System::PartType type, uint32_t amt, uint64_t time_ordered);
    };
    class Machine {
        bool is_down, is_busy;
        uint64_t reopen_time;
        System::Part cur;
        std::queue<System::Batch> input, output;
    public:
        Machine();
        void toggle_status();
        void load_input(System::Part part, uint64_t ts);
        uint64_t load_machine(uint64_t ts);
        void load_output(uint64_t ts);
        System::Part remove_part();
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
    System::Batch buffer;
    std::queue<System::Order> order[7];
    uint32_t backlog[7];
    System::Demand *demand[7];
    uint64_t changeover_time[4][7];
    uint64_t process_time[4][7];
public:
    uint64_t cur_time;
    System(int32_t seed, std::string config);
    void generate_order();
    bool fulfil_order(System::PartType type);
    void ship_order(System::PartType type);
    void enter_queue(Part part, uint32_t stage, uint32_t machine);
    void enter_machine(uint32_t stage, uint32_t machine);
    void display_status(std::ostream& os);
    void display_config(std::ostream& os);
    ~System();
};
#endif
