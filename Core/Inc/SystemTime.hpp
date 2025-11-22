#pragma once
#include <cstdint>

class SystemTime {
    public:
        SystemTime();
        ~SystemTime() = default;
        void tick();
        uint32_t getTick() const;

    private:
        volatile uint32_t sinceBootMs;
};
