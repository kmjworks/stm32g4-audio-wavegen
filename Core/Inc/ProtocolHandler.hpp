#pragma once

#include <cstdint>
#include <protocol/protocol.hpp>

#include "IPacketReceiver.hpp"

class Packetizer;
class SystemTime;

class ProtocolHandler : public IPacketReceiver {
    public:
        ProtocolHandler(SystemTime& systemTime, Packetizer* packetizer = nullptr);

        void onLUTUpdate(const LUTUpdatePacket& packet) override;
        void onWavegenOff(const WavegenOffPacket& packet) override;
        void onWavegenConfig(const WavegenConfigPacket& packet) override;
        void onFeedbackRequest(const FeedbackRequestPacket& packet) override;

        void setPacketizer(Packetizer* pktizer);
        void sendPeriodicFeedback();

    private:
        FeedbackPacket buildFeedbackPacket();
        bool sendFeedbackPacket();

        SystemTime& time;
        Packetizer* packetizer;

        uint32_t frequencyHz;
        uint16_t amplitudeMilliVolts;
        int16_t offsetMilliVolts;
        uint8_t lutUpdatesSinceBoot;
        uint8_t dmaErrorsSinceBoot;
        bool wavegenActive;
        uint8_t feedbackCounter;
        uint32_t lastFeedbackMs;
        bool pendingImmediateFeedback;
};
