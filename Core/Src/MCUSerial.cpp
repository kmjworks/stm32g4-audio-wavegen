#include "MCUSerial.hpp"

#include <cstring>
#include <utility>
#include "DMAController.hpp"

namespace internal {
    namespace error_detection {
        uint16_t crc16Ccitt(const uint8_t* data, size_t size) {
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
                if (code == 0) {
                    return 0;
                }
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
            size_t writeIndex = 1;
            size_t codeIndex = 0;
            uint8_t code = 1;

            for (size_t i = 0; i < length; ++i) {
                if (input[i] == 0) {
                    out[codeIndex] = code;
                    codeIndex = writeIndex++;
                    code = 1;
                } else {
                    out[writeIndex++] = input[i];
                    code++;
                    if (code == 0xFF) {
                        out[codeIndex] = code;
                        codeIndex = writeIndex++;
                        code = 1;
                    }
                }
            }

            out[codeIndex] = code;
            return writeIndex;
        }
    }
};



MCUSerial::MCUSerial(USART_TypeDef* usartHandle, RBuffer<uint8_t>& buffer, std::shared_ptr<IDMA> dmaCtrlRx, std::shared_ptr<IDMA> dmaCtrlTx) 
: usartInternal(usartHandle), dmaRx(std::move(dmaCtrlRx)), dmaTx(std::move(dmaCtrlTx)), rxBuffer(buffer), rxBufDelimiterFoundIndex(0)  {
    const uint32_t rxPeriphAddr = LL_USART_DMA_GetRegAddr(usartInternal, LL_USART_DMA_REG_DATA_RECEIVE);
    const uint32_t txPeriphAddr = LL_USART_DMA_GetRegAddr(usartInternal, LL_USART_DMA_REG_DATA_TRANSMIT);

    auto addr = (uint32_t)(uintptr_t)rxBuffer.getRawBuffer();
    
    dmaRx->setPeripheralAddress(rxPeriphAddr);
    dmaRx->changeDestinationAndDataLength(addr, rxBuffer.getInternalSize());
    dmaRx->enable();

    dmaTx->setPeripheralAddress(txPeriphAddr);

    LL_USART_EnableDMAReq_RX(usartInternal);
    LL_USART_EnableDMAReq_TX(usartInternal);
}

void MCUSerial::write(const uint8_t* dat) {
    if (dat == nullptr) {
        return;
    }

    const size_t length = std::strlen(reinterpret_cast<const char*>(dat));
    write(dat, length);
}

void MCUSerial::write(const uint8_t* data, size_t length) {
    if (data == nullptr || length > txDmaBuffer.size() || length == 0) {
        return;
    }

    if (dmaTx->isBusy()) {
        return;
    }

    dmaTx->disable();

    memcpy(txDmaBuffer.data(), data, length);

    dmaTx->changeDestinationAndDataLength((uint32_t)(uintptr_t)txDmaBuffer.data(), length);
    dmaTx->enable();
}

void MCUSerial::receive() {
    checkAndProcess();
}


bool MCUSerial::isTxBusy() const {
    return dmaTx->isBusy();
}

void MCUSerial::setReceiver(IFrameReceiver* newReceiver) {
    receiver = newReceiver;
}

void MCUSerial::checkAndProcess() {
    size_t dmaWritePos = dmaRx->getCurrentWritePos();
    rxBuffer.updateHeadUnsafe(dmaWritePos);

    uint8_t byte = 0;

    while (rxBuffer.pop(&byte, 1)) {
        if (byte == 0x00) {
            if (rxBufDelimiterFoundIndex > 0) {
                if (receiver != nullptr) {
                    receiver->onCompleteRawFrameReceived(rxBufDelimiterFound.data(), rxBufDelimiterFoundIndex);
                }
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
