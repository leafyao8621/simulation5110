#include <iostream>
#include <fstream>
#include <cstdlib>
#include "engine.h"
#include "system.h"
#include "stats.h"

int main(int argc, const char** argv) {
    // System::DemandKDE dmd("data/C17.dmd", 1);
    // for (int i = 0; i < 100000; std::cout << dmd() << '\n', i++);
    if (argc < 6) {
        std::cout << "config max_time interim backlog\n";
        return 1;
    }
    System sys(1, argv[1]);
    sys.display_config(std::cout);
    Stats stats(argv[1]);
    Engine engine(&sys, &stats, atoi(argv[2]), argv[4]);
    engine.run(0, 0, atoi(argv[3]), std::cout);
    sys.display_status(std::cout);
    stats.log(std::cout, sys.cur_time);
    return 0;
}
