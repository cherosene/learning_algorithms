#include "utility.h"
#include <iostream>

void print(const char* msg) {
    static unsigned int logCounter = 0;
    std::cout << "Log " << logCounter << ": " << msg << std::endl;
    logCounter++;
}

void print(int n) {
    print(std::to_string(n).c_str());
}