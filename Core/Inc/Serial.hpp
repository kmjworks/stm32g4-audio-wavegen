#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>

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
        virtual void write(const uint8_t* data, size_t length) = 0;

        virtual void write(const uint8_t* data) {
            if (data == nullptr) {
                return;
            }
            const size_t length = std::strlen(reinterpret_cast<const char*>(data));
            write(data, length);
        }

        virtual void receive() = 0;
        virtual void reconfigure(const Parity& newParity, const BaudRate& newBaud) = 0;

        ~ISerial() = default;
};



