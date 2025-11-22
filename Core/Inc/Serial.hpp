#pragma once
#include <cstdint>

enum class BaudRate {
    BAUD_9600 = 0,
    BAUD_115200,
    BAUD_460800,
};

enum class Parity {
    NONE = 0,
    ENABLED
};

class ISerial {
    public:
        virtual void write(const uint8_t* data) = 0;
        virtual void receive() = 0;
        virtual void reconfigure(const Parity& newParity, const BaudRate& newBaud) = 0;

        ~ISerial() = default;
};



