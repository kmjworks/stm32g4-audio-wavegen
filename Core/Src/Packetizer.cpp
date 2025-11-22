#include "Packetizer.hpp"

#include <cstring>

namespace {
    bool expectsAcknowledgement(const Command command) {
        switch (command) {
            case Command::ACK:
            case Command::NACK:
            case Command::DEVICE_FEEDBACK:
                return false;
            default:
                return true;
        }
    }
}

Packetizer::Packetizer(ISerial& serialInterface) : serial(serialInterface) {}

void Packetizer::onCompleteRawFrameReceived(const uint8_t* data, size_t size) {
    if (data == nullptr or size == 0 or size > encodedBuffer.size()) {
        return;
    }

    const size_t decodedSize = internal::COBS::decode(data, size, decodeBuffer.data());
    if (decodedSize < (CRC_SIZE + 1) or decodedSize > decodeBuffer.size()) {
        return;
    }

    const size_t payloadSize = decodedSize - CRC_SIZE;
    const auto receivedCrc = static_cast<uint16_t>(
        static_cast<uint16_t>(decodeBuffer[payloadSize]) |
        static_cast<uint16_t>(decodeBuffer[payloadSize + 1] << 8)
    );
    const uint16_t computedCrc = internal::error_detection::crc16Ccitt(decodeBuffer.data(), payloadSize);

    if (receivedCrc != computedCrc) {
        return;
    }

    const auto command = static_cast<Command>(decodeBuffer[0]);
    const uint8_t counter = decodeBuffer[payloadSize - 1];
    const bool handled = dispatchPayload(command, decodeBuffer.data(), payloadSize);

    if (!expectsAcknowledgement(command)) {
        return;
    }

    if (handled) {
        sendAck(command, counter);
    } else {
        sendNack(command, counter);
    }
}

bool Packetizer::sendFeedback(const FeedbackPacket& packet) {
    return sendPacket(packet);
}

bool Packetizer::sendAck(Command ackedPacket, uint8_t counter) {
    ACKPacket packet{};
    packet.packetIdentifier = static_cast<uint8_t>(Command::ACK);
    packet.ackedPacket = static_cast<uint8_t>(ackedPacket);
    packet.counter = counter;
    return sendPacket(packet);
}

bool Packetizer::sendNack(Command ackedPacket, uint8_t counter) {
    NACKPacket packet{};
    packet.packetIdentifier = static_cast<uint8_t>(Command::NACK);
    packet.ackedPacket = static_cast<uint8_t>(ackedPacket);
    packet.counter = counter;
    return sendPacket(packet);
}

bool Packetizer::dispatchPayload(const Command command, const uint8_t* payload, size_t payloadSize) {
    if (payload == nullptr) {
        return false;
    }

    switch (command) {
        case Command::CMD_UPDATE_LUT: {
            if (payloadSize != sizeof(LUTUpdatePacket)) {
                return false;
            }
            LUTUpdatePacket packet{};
            std::memcpy(&packet, payload, sizeof(packet));
            //receiver.onLUTUpdate(packet);
            return true;
        }
        case Command::CMD_WAVEGEN_OFF: {
            if (payloadSize != sizeof(WavegenOffPacket)) {
                return false;
            }
            WavegenOffPacket packet{};
            std::memcpy(&packet, payload, sizeof(packet));
            //receiver.onWavegenOff(packet);
            return true;
        }
        case Command::CMD_WAVEGEN_CONFIG: {
            if (payloadSize != sizeof(WavegenConfigPacket)) {
                return false;
            }
            WavegenConfigPacket packet{};
            std::memcpy(&packet, payload, sizeof(packet));
            //receiver.onWavegenConfig(packet);
            return true;
        }
        case Command::CMD_FEEDBACK_REQ: {
            if (payloadSize != sizeof(FeedbackRequestPacket)) {
                return false;
            }
            FeedbackRequestPacket packet{};
            std::memcpy(&packet, payload, sizeof(packet));
            //receiver.onFeedbackRequest(packet);
            return true;
        }
        default:
            return false;
    }
}

template <typename T>
bool Packetizer::sendPacket(const T& packet) {
    constexpr size_t payloadSize = sizeof(T);
    static_assert(payloadSize + CRC_SIZE <= MAX_PACKET_SIZE, "Payload exceeds configured maximum packet size");

    if ((payloadSize + CRC_SIZE + 1) > txBuffer.size()) {
        return false;
    }

    std::memcpy(txBuffer.data(), &packet, payloadSize);

    const uint16_t crc = internal::error_detection::crc16Ccitt(txBuffer.data(), payloadSize);
    txBuffer[payloadSize] = static_cast<uint8_t>(crc & 0xFF);
    txBuffer[payloadSize + 1] = static_cast<uint8_t>((crc >> 8) & 0xFF);

    const size_t encodedLength = internal::COBS::encode(txBuffer.data(), payloadSize + CRC_SIZE, encodedBuffer.data());
    if (encodedLength == 0 or (encodedLength + 1) > encodedBuffer.size()) {
        return false;
    }

    encodedBuffer[encodedLength] = 0x00; // frame delimiter
    serial.write(encodedBuffer.data(), encodedLength + 1);
    return true;
}

// Explicit template instantiations for the packet types used by the protocol
template bool Packetizer::sendPacket<ACKPacket>(const ACKPacket& packet);
template bool Packetizer::sendPacket<NACKPacket>(const NACKPacket& packet);
template bool Packetizer::sendPacket<FeedbackPacket>(const FeedbackPacket& packet);
