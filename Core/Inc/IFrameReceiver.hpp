#pragma once
#include <cstdint>
#include <cstddef>

class IFrameReceiver {
    public:
        virtual void onCompleteRawFrameReceived(const uint8_t* data, size_t size) = 0;
        virtual ~IFrameReceiver() = default;
};