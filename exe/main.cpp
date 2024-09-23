#include <iostream>
#include "chronista/2v2pl.hpp"
#include <sstream>

using namespace chronista;

int main() {
    
    std::cout << "Welcome to Chronista v0.1" << std::endl;
    Scheduler2V2PL scheduler;
    std::istringstream iss2("T2: r(1.2.3.4)\n"
                            "T1: w(1.2.3.4)\n"
                            "T2: w(1.2.3.4)\n"
                            "T1: r(1.2.3)\n"
                            "T1: c\n"
                            "T3: r(1.2.3.4)\n"
                            "T2: c\n"
                            "T3: w(1.2)\n"
                            "T3: c\n");

    Schedule c(iss2);

    auto a = scheduler.schedule(c);

    for (auto b : a.get_schedule()) {

        std::cout << b.get_operation_string() << std::endl;

    }
    
}
