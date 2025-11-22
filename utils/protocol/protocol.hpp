#pragma once
#include <cstdint>

#define LUT_SIZE 128
#define CRC_SIZE 2
#define MAX_PACKET_SIZE 514+2+3 // COBS overhead, CRC is 2 bytes - the largest packet is the LUT packet (514)


#pragma pack(push,1)

enum class Command : uint8_t {
    CMD_UPDATE_LUT      = 0x00,
    CMD_WAVEGEN_OFF     = 0x01,

    DEVICE_FEEDBACK     = 0x20,

    ACK                 = 0x0A,
    NACK                = 0x1A,
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

struct FeedbackPacket {
    uint8_t packetIdentifier;                   // 0x20 - DEVICE_FEEDBACK
    uint32_t    sineWaveGenFrequency;           // Current frequency in Hz at which the sine wave is generated at
    uint8_t     lutUpdatesSinceBoot;            // How many times the sine wave's LUT has been updated
    uint8_t     dmaErrorsSinceBoot;             // DMA errors since boot
    uint8_t     waveGenActive       : 1;        // Set if sine wave is being generated via DAC
    uint8_t     usartDmaStatusOk     : 1;       // Set if USART2's DMA is initialized & operational and has not encountered an unrecoverable error
    uint8_t     dacStatusOk         : 1;        // Set if DAC is initialized & operational and has not encountered an unrecoverable error
    uint8_t     counter;
};





#pragma pack(pop)