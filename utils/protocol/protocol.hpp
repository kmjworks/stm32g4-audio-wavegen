#pragma once
#include <cstdint>
#include <cstddef>

#define LUT_SIZE 128
#define CRC_SIZE 2
#define MAX_PACKET_SIZE 514 + CRC_SIZE + 3 + 1 // CRC is 2 bytes, COBS worst-case overhead is 3 bytes, plus 1 for the frame delimiter


#pragma pack(push,1)

enum class Command : uint8_t {
    CMD_UPDATE_LUT      = 0x00,
    CMD_WAVEGEN_OFF     = 0x01,
    CMD_WAVEGEN_CONFIG  = 0x02,   // Configure the sine generator (frequency, amplitude, offset)
    CMD_FEEDBACK_REQ    = 0x03,   // Request a feedback snapshot from the device

    DEVICE_FEEDBACK     = 0x20,

    ACK                 = 0x0A,
    NACK                = 0x1A,
};


struct WavegenOffPacket {
    uint8_t packetIdentifier;                   // 0x01 - CMD_WAVEGEN_OFF
    uint8_t counter;                            // Packet counter
};

struct WavegenConfigPacket {
    uint8_t  packetIdentifier;                  // 0x02 - CMD_WAVEGEN_CONFIG
    uint32_t sineWaveGenFrequency;              // Desired frequency in Hz
    uint16_t amplitudeMilliVolts;               // Desired peak-to-peak amplitude in mV
    int16_t  offsetMilliVolts;                  // DC offset applied to the waveform (center point) in mV
    uint8_t  counter;                           // Packet counter
};

struct LUTUpdatePacket {
    uint8_t packetIdentifier;                   // 0x00 - CMD_UPDATE_LUT
    uint32_t waveLUT[LUT_SIZE];                 // Look-up table for the sine sound wave's points
    uint8_t counter;                            // Packet counter - monotonously increasing, host uses it for checking whether packets have been lost during communication or not
};

struct ACKPacket {
    uint8_t packetIdentifier;                   // 0x0A - ACK
    uint8_t ackedPacket;                        // The identifier of the packet being acknowledged
    uint8_t counter;                        
};

struct NACKPacket {
    uint8_t packetIdentifier;                   // 0x1A - NACK
    uint8_t ackedPacket;                        // The identifier of the packet not being acknowledged
    uint8_t counter;                
};

struct FeedbackRequestPacket {
    uint8_t packetIdentifier;                   // 0x03 - CMD_FEEDBACK_REQ
    uint8_t counter;                            // Packet counter
};

enum FeedbackStatus : uint8_t {
    FEEDBACK_WAVEGEN_ACTIVE  = 1 << 0,
    FEEDBACK_USART_DMA_OK    = 1 << 1,
    FEEDBACK_DAC_OK          = 1 << 2,
};

struct FeedbackPacket {
    uint8_t packetIdentifier;                   // 0x20 - DEVICE_FEEDBACK
    uint32_t    sineWaveGenFrequency;           // Current frequency in Hz at which the sine wave is generated at
    uint16_t    amplitudeMilliVolts;            // Current peak-to-peak amplitude in millivolts
    int16_t     offsetMilliVolts;               // Current offset in millivolts applied to the waveform
    uint8_t     lutUpdatesSinceBoot;            // How many times the sine wave's LUT has been updated
    uint8_t     dmaErrorsSinceBoot;             // DMA errors since boot
    uint8_t     statusFlags;                    // Bitmask of FeedbackStatus
    uint8_t     counter;                        // Packet counter
};





#pragma pack(pop)
