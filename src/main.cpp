#include <iostream>
#include "engine.h"
#include "system.h"
#include "stats.h"

int main() {
    // System::DemandKDE dmd("data/B15.dmd", 1);
    // for (int i = 0; i < 100000; std::cout << dmd() << '\n', i++);
    System sys(1, "config/c04");
    sys.display_config(std::cout);
    Stats stats("config/c04");
    Engine engine(&sys, &stats, 525600 * 5);
    engine.run(0, 0);
    sys.display_status(std::cout);
    stats.log(std::cout, sys.cur_time);
    return 0;
}
