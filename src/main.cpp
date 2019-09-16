#include <iostream>
#include "engine.h"
#include "system.h"
#include "stats.h"

int main() {
    // System::DemandKDE dmd("data/B15.dmd", 1);
    // for (int i = 0; i < 100000; std::cout << dmd() << '\n', i++);
    System sys(1, "config/c01");
    sys.display_config(std::cout);
    Stats stats;
    Engine engine(&sys, &stats, 20000);
    engine.run();
    return 0;
}
