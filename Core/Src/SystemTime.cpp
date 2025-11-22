#include "SystemTime.hpp"

SystemTime::SystemTime() : sinceBootMs(0) {}

void SystemTime::tick() {
    sinceBootMs++;
}