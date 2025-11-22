#pragma once
#include <protocol/protocol.hpp>

class IPacketReceiver {
    public:
        virtual void onLUTUpdate(const LUTUpdatePacket& packet) = 0;
        virtual void onWavegenOff(const WavegenOffPacket& packet) = 0;
        virtual void onWavegenConfig(const WavegenConfigPacket& packet) = 0;
        virtual void onFeedbackRequest(const FeedbackRequestPacket& packet) = 0;
        virtual ~IPacketReceiver() = default;
};

