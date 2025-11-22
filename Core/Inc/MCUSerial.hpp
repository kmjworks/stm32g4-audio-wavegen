#pragma once
#include "Serial.hpp"
#include <cstddef>

namespace internal {
    namespace error_detection {
        uint16_t crc16Ccitt(uint8_t* data, size_t size);
    }
};


class MCUSerial : public ISerial {
    public:
        MCUSerial(const Parity& initParity, const BaudRate& initBaud);
        void write(const uint8_t* data) override;
        void receive() override;
        void reconfigure(const Parity& newParity, const BaudRate& newBaud) override;

    private:
        Parity currentParity;
        BaudRate  currentBaud;
        


};