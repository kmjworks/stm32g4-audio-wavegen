#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "IFrameReceiver.hpp"
//#include "IPacketReceiver.hpp"
#include "Serial.hpp"
#include "MCUSerial.hpp"
#include <protocol/protocol.hpp>

class Packetizer : public IFrameReceiver {
    public:
        Packetizer(ISerial& serialInterface);

        void onCompleteRawFrameReceived(const uint8_t* data, size_t size) override;

        bool sendFeedback(const FeedbackPacket& packet);
        bool sendAck(Command ackedPacket, uint8_t counter);
        bool sendNack(Command ackedPacket, uint8_t counter);

    private:
        template <typename T>
        bool sendPacket(const T& packet);

        bool dispatchPayload(const Command command, const uint8_t* payload, size_t payloadSize);

        ISerial& serial;
        //IPacketReceiver& receiver;

        std::array<uint8_t, MAX_PACKET_SIZE> decodeBuffer;
        std::array<uint8_t, MAX_PACKET_SIZE> txBuffer;
        std::array<uint8_t, MAX_PACKET_SIZE> encodedBuffer;
};
