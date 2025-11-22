#include "DMAController.hpp"

DMAController::DMAController(DMA_TypeDef* dmaInstance, uint32_t channel, uint32_t destinationAddr, size_t bufSize) : dma(dmaInstance), ch(channel), bufferSize(bufSize), isEnabled(false) {

    LL_DMA_DisableChannel(dma, ch);
    LL_DMA_SetMemoryAddress(dma, channel, destinationAddr);
    LL_DMA_SetDataLength(dma, channel, bufferSize);
}


size_t DMAController::getCurrentWritePos() {
    return bufferSize - LL_DMA_GetDataLength(dma, ch);
}

bool DMAController::changeDestinationAndDataLength(uint32_t destination, size_t size) {
    if (isEnabled) {
        return (not isEnabled);
    }

    bufferSize = size;
    LL_DMA_SetMemoryAddress(dma, ch, destination);
    LL_DMA_SetDataLength(dma, ch,size);
    return true;
}

void DMAController::enable() {
    LL_DMA_EnableChannel(dma, ch);
    isEnabled = true;
}

void DMAController::disable() {
    LL_DMA_DisableChannel(dma, ch);
    isEnabled = false;
}
