#include "core.h"
#include <iostream>

namespace core {
    void printMessage(const char* message) {
        std::cout << "Message from core library: " << message << std::endl;
    }
}