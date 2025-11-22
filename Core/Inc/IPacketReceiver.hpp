#pragma once
#include <protocol/protocol.hpp>

class PacketReceiver {
    public:
        void onLUTUpdate(const LUTUpdatePacket& packet);
        ~PacketReceiver() = default;

};

