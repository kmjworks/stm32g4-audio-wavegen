#pragma once
#include <cstdint>

class IDMA {
    public:
        virtual size_t getCurrentWritePos() = 0;
        virtual bool changeDestinationAndDataLength(uint32_t destination, size_t size) = 0;
        virtual void enable() = 0;
        virtual void disable() = 0;
        virtual ~IDMA() = default;
};