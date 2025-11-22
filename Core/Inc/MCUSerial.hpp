#pragma once
#include "Serial.hpp"
#include "stm32g4xx_ll_usart.h"
#include <structures/RBuffer.hpp>
#include <cstddef>

namespace internal {
    namespace error_detection {
        uint16_t crc16Ccitt(uint8_t* data, size_t size);
    }
};


class MCUSerial : public ISerial {
    public:
        MCUSerial(USART_TypeDef* usartHandle);
        void write(const uint8_t* data) override;
        void receive() override;
        void checkAndProcess();

    private:
        USART_TypeDef* usartInternal; 

};