#include "MCUSerial.hpp"

#include <utility>
#include "DMAController.hpp"

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

    namespace COBS {
        size_t decode(const uint8_t* input, size_t length, uint8_t* out) {
            const uint8_t* end = input + length;
            uint8_t* writePtr = out;

            while (input < end) {
                int code = *input++;
                if (input + code - 1 > end) {
                    return 0;
                }

                for (int i = 1; i < code; ++i) {
                    *writePtr++ = *input++;
                }

                if (code < 0xFF && input < end) {
                    *writePtr++ = 0;
                }
            }

            return writePtr - out;
        }

        size_t encode(const uint8_t* input, size_t length, uint8_t* out) {
            size_t writeIndex = 0, codeIndex = 0, nextDelimiterIndex = 0;

            out[writeIndex++] = 0;


            for (size_t i = 0; i < length; ++i) {
                if (input[i] == 0) {
                    out[codeIndex] = (i - nextDelimiterIndex) + 1;
                    codeIndex = writeIndex;

                    out[writeIndex++] = 0;
                    nextDelimiterIndex = i++;
                } else {
                    out[writeIndex++] = input[i];
                }
            }

            out[codeIndex] = (length - nextDelimiterIndex) + 1;
            return writeIndex;
        }
    }
};



MCUSerial::MCUSerial(USART_TypeDef* usartHandle, IFrameReceiver* rawDataReceiver, RBuffer<uint8_t>& buffer, std::shared_ptr<IDMA> dmaCtrlRx, std::shared_ptr<IDMA> dmaCtrlTx) 
: usartInternal(usartHandle), receiver(rawDataReceiver), dmaRx(std::move(dmaCtrlRx)), dmaTx(std::move(dmaCtrlTx)), rxBuffer(buffer), rxBufDelimiterFoundIndex(0)  {
    auto addr = (uint32_t)(uintptr_t)rxBuffer.getRawBuffer();
    
    dmaRx->changeDestinationAndDataLength(addr, rxBuffer.getInternalSize());
    dmaRx->enable();

    LL_USART_EnableDMAReq_RX(usartInternal);
    LL_USART_EnableDMAReq_TX(usartInternal);
}

void MCUSerial::write(const uint8_t* dat) {
    
}

void MCUSerial::write(const uint8_t* data, size_t length) {
    if (length > txDmaBuffer.size() or length == 0) {
        return;
    }

    memcpy(txDmaBuffer.data(), data, length);

    dmaTx->changeDestinationAndDataLength((uint32_t)(uintptr_t)txDmaBuffer.data(), length);
    dmaTx->enable();
}

void MCUSerial::receive() {
    checkAndProcess();
}


bool MCUSerial::isTxBusy() {
    return false;
}

void MCUSerial::checkAndProcess() {
    size_t dmaWritePos = dmaRx->getCurrentWritePos();
    rxBuffer.updateHeadUnsafe(dmaWritePos);

    uint8_t byte = 0;

    while (rxBuffer.pop(&byte, 1)) {
        if (byte == 0x00) {
            if (rxBufDelimiterFoundIndex > 0) {
                receiver->onCompleteRawFrameReceived(rxBufDelimiterFound.data(), rxBufDelimiterFoundIndex);
            }

            rxBufDelimiterFoundIndex = 0;
        } else {
            if (rxBufDelimiterFoundIndex < rxBufDelimiterFound.size()) {
                rxBufDelimiterFound[rxBufDelimiterFoundIndex++] = byte;
            } else {
                rxBufDelimiterFoundIndex = 0;
            }
        }
    }
}