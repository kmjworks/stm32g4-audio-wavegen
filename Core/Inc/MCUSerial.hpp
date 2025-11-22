#pragma once
#include <cstddef>
#include <array>
#include "Serial.hpp"
#include "IDMA.hpp"
#include "IFrameReceiver.hpp"
#include <structures/RBuffer.hpp>

#include "stm32g4xx_ll_usart.h"

#define MAX_SIZE 514+2+3+1

namespace internal {
    namespace error_detection {
        uint16_t crc16Ccitt(const uint8_t* data, size_t size);
    }

    namespace COBS {
        size_t decode(const uint8_t* input, size_t length, uint8_t* out);
        size_t encode(const uint8_t* input, size_t length, uint8_t* out);
    }
};


class MCUSerial : public ISerial {
    public:
        MCUSerial(USART_TypeDef* usartHandle, IFrameReceiver* rawDataReceiver, RBuffer<uint8_t>& buffer, std::shared_ptr<IDMA> dmaCtrlRx, std::shared_ptr<IDMA> dmaCtrlTx);

        [[deprecated("Prefer the overloaded method ::write(const uint8_t* data, size_t length)")]]
        void write(const uint8_t* data) override;
        
        void write(const uint8_t* data, size_t length) override;
        void receive() override;
        void reconfigure(const Parity& newParity, const BaudRate& newBaud) override;

        bool isTxBusy();
    private:
        void checkAndProcess();

        USART_TypeDef* usartInternal;
        IFrameReceiver* receiver;
        std::shared_ptr<IDMA> dmaRx;
        std::shared_ptr<IDMA> dmaTx;
        RBuffer<uint8_t>& rxBuffer;

        std::array<uint8_t, MAX_SIZE> rxBufDelimiterFound;
        size_t rxBufDelimiterFoundIndex;

        std::array<uint8_t, MAX_SIZE> txDmaBuffer;

};
