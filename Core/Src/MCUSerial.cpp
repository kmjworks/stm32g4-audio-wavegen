#include "MCUSerial.hpp"

namespace internal {
    namespace error_detection {
        uint16_t crc16Ccitt(uint8_t* data, size_t size) {
            uint8_t x;
            uint16_t crc = 0xFFF;

            for (size_t i = 0; i < size; ++i) {
                x = (uint8_t) (crc >> 8 ^ data[i]);
                x ^= x >> 4;
                crc = (crc << 8) ^ ((uint16_t) (x << 12) ^ ((uint16_t) (x << 5)) ^ (uint16_t)x); 
            }
            return crc;
        }
    }
};



MCUSerial::MCUSerial(USART_TypeDef* usartHandle) : usartInternal(usartHandle) {
    // WIP
}

void MCUSerial::write(const uint8_t* dat) {

}

void MCUSerial::receive() {

}

void MCUSerial::checkAndProcess() {
    // e.g. DMAController->getAvailableDataBytes()
    
}