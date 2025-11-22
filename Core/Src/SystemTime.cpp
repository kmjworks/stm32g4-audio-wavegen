#include "SystemTime.hpp"

SystemTime::SystemTime() : sinceBootMs(0) {}

void SystemTime::tick() {
    sinceBootMs++;
}

uint32_t SystemTime::getTick() const {
    return sinceBootMs;
}
