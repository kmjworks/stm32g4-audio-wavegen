#include "DMAController.hpp"

DMAController::DMAController(DMA_TypeDef* dmaInstance, uint32_t channel, uint32_t destinationAddr, size_t bufSize) : dma(dmaInstance), ch(channel), bufferSize(bufSize) {

    LL_DMA_DisableChannel(dma, ch);
    LL_DMA_SetMemoryAddress(dma, channel, destinationAddr);
    LL_DMA_SetDataLength(dma, channel, bufferSize);
}


size_t DMAController::getCurrentWritePos() {
    return bufferSize - LL_DMA_GetDataLength(dma, ch);
}

bool DMAController::changeDestinationAndDataLength(uint32_t destination, size_t size) {
    bufferSize = size;
    LL_DMA_SetMemoryAddress(dma, ch, destination);
    LL_DMA_SetDataLength(dma, ch,size);
    return true;
}

void DMAController::setPeripheralAddress(uint32_t address) {
    LL_DMA_SetPeriphAddress(dma, ch, address);
}

void DMAController::enable() {
    LL_DMA_EnableChannel(dma, ch);
}

void DMAController::disable() {
    LL_DMA_DisableChannel(dma, ch);
}

bool DMAController::isBusy() const {
    if (LL_DMA_IsEnabledChannel(dma, ch) == 0) {
        return false;
    }

    const uint32_t mode = LL_DMA_GetMode(dma, ch);
    if (mode == LL_DMA_MODE_CIRCULAR) {
        return true;
    }

    return LL_DMA_GetDataLength(dma, ch) != 0;
}
