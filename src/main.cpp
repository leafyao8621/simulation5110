#include <iostream>
#include "engine.h"
#include "system.h"
#include "stats.h"

int main() {
    System sys(1);
    Stats stats;
    Engine engine(&sys, &stats, 5256000);
    engine.run();
}
