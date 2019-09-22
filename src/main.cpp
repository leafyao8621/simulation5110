#include <iostream>
#include "engine.h"
#include "system.h"
#include "stats.h"

int main() {
    // System::DemandKDE dmd("data/C17.dmd", 1);
    // for (int i = 0; i < 100000; std::cout << dmd() << '\n', i++);
    System sys(1, "config/c04");
    sys.display_config(std::cout);
    Stats stats("config/c04");
    Engine engine(&sys, &stats, 5256000);
    engine.run(0, 0);
    sys.display_status(std::cout);
    stats.log(std::cout, sys.cur_time);
    return 0;
}
