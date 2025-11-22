#include "ProtocolHandler.hpp"

#include "Packetizer.hpp"
#include "SystemTime.hpp"

ProtocolHandler::ProtocolHandler(SystemTime& systemTime, Packetizer* pktizer)
: time(systemTime),
  packetizer(pktizer),
  frequencyHz(0),
  amplitudeMilliVolts(0),
  offsetMilliVolts(0),
  lutUpdatesSinceBoot(0),
  dmaErrorsSinceBoot(0),
  wavegenActive(false),
  feedbackCounter(0),
  lastFeedbackMs(0),
  pendingImmediateFeedback(false) {}

void ProtocolHandler::onLUTUpdate(const LUTUpdatePacket& /*packet*/) {
    ++lutUpdatesSinceBoot;
    wavegenActive = true;
}

void ProtocolHandler::onWavegenOff(const WavegenOffPacket& /*packet*/) {
    wavegenActive = false;
}

void ProtocolHandler::onWavegenConfig(const WavegenConfigPacket& packet) {
    frequencyHz = packet.sineWaveGenFrequency;
    amplitudeMilliVolts = packet.amplitudeMilliVolts;
    offsetMilliVolts = packet.offsetMilliVolts;
    wavegenActive = true;
}

void ProtocolHandler::onFeedbackRequest(const FeedbackRequestPacket& /*packet*/) {
    pendingImmediateFeedback = true;
}

void ProtocolHandler::setPacketizer(Packetizer* pktizer) {
    packetizer = pktizer;
}

void ProtocolHandler::sendPeriodicFeedback() {
    const uint32_t now = time.getTick();
    // 30 Hz -> every ~33 ms
    const bool intervalElapsed = (now - lastFeedbackMs) >= 33U;
    if (pendingImmediateFeedback || intervalElapsed) {
        sendFeedbackPacket();
    }
}

FeedbackPacket ProtocolHandler::buildFeedbackPacket() {
    FeedbackPacket packet{};
    packet.packetIdentifier = static_cast<uint8_t>(Command::DEVICE_FEEDBACK);
    packet.sineWaveGenFrequency = frequencyHz;
    packet.amplitudeMilliVolts = amplitudeMilliVolts;
    packet.offsetMilliVolts = offsetMilliVolts;
    packet.lutUpdatesSinceBoot = lutUpdatesSinceBoot;
    packet.dmaErrorsSinceBoot = dmaErrorsSinceBoot;
    packet.statusFlags = 0;

    if (wavegenActive) {
        packet.statusFlags |= FEEDBACK_WAVEGEN_ACTIVE;
    }
    packet.statusFlags |= FEEDBACK_USART_DMA_OK;
    packet.statusFlags |= FEEDBACK_DAC_OK;

    packet.counter = feedbackCounter++;
    return packet;
}

bool ProtocolHandler::sendFeedbackPacket() {
    if (packetizer == nullptr || packetizer->isTransmitBusy()) {
        return false;
    }

    FeedbackPacket packet = buildFeedbackPacket();
    const bool sent = packetizer->sendFeedback(packet);
    if (sent) {
        lastFeedbackMs = time.getTick();
        pendingImmediateFeedback = false;
    }
    return sent;
}
