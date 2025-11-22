#pragma once
#include <cstdint>

#define LUT_SIZE 128
#define CRC_SIZE 2


#pragma pack(push,1)

enum class Command : uint8_t {
    CMD_UPDATE_LUT      = 0x00,
    CMD_WAVEGEN_OFF     = 0x01,

    DEVICE_FEEDBACK     = 0x20,

    ACK                 = 0x0A,
    NACK                = 0x1A,
};


struct LUTUpdatePacket {
    uint8_t packetIdentifier;
    uint32_t waveLUT[LUT_SIZE];
    uint8_t crc;
    uint8_t counter;
};

struct ACKPacket {
    uint8_t packetIdentifier;
    uint8_t ackedPacket;
    uint8_t counter;
};

struct NACKPacket {
    uint8_t packetIdentifier;
    uint8_t ackedPacket;
    uint8_t counter;
};

struct FeedbackPacket {
    uint8_t packetIdentifier;
    uint32_t    sineWaveGenFrequency;
    uint8_t     lutUpdatesSinceBoot;
    uint8_t     dmaErrorsSinceBoot;
    uint8_t     waveGenActive   : 1;
    uint8_t     dmaStatusOk     : 1;
    uint8_t     dacStatusOk     : 1;
    uint8_t     counter;
};





#pragma pack(pop)