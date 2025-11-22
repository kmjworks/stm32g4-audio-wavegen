#pragma once
#include <cstdint>

class SystemTime {
    public:
        SystemTime();
        ~SystemTime() = default;
        void tick();
        int64_t getTick;

    private:
        int64_t sinceBootMs;
};