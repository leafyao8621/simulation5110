#include <iostream>
#include "engine.h"
#include "system.h"
#include "stats.h"

int main() {
    System sys(1, "config/c01");
    sys.display_config(std::cout);
    sys.display_status(std::cout);
    // Stats stats;
    // Engine engine(&sys, &stats, 5256000);
    // engine.run();
}
