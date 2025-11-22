#pragma once
#include "IDMA.hpp"
#include "stm32g4xx_ll_dma.h"

class DMAController : public IDMA {
    public:
        DMAController(DMA_TypeDef* dmaInstance, uint32_t channel, uint32_t destinationAddr, size_t bufSize);

        size_t getCurrentWritePos() override;
        bool changeDestinationAndDataLength(uint32_t destination, size_t size) override;
        void enable() override;
        void disable() override;

    private:
        DMA_TypeDef* dma;
        uint32_t ch;
        size_t bufferSize;
        volatile bool isEnabled;
};