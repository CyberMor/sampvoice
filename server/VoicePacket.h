/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cstdint>
#include <cstddef>

#include <util/memory.hpp>

#pragma pack(push, 1)

struct VoicePacket
{
    uint32_t hash;
    uint32_t svrkey;
    uint32_t packet;
    uint32_t stream;
    uint16_t sender;
    uint16_t length;
    uint32_t packid;
    uint8_t  data[];

    uint32_t GetFullSize() const noexcept;
    bool CheckHeader() const noexcept;
    void CalcHash() noexcept;
};

#pragma pack(pop)

static_assert(offsetof(VoicePacket, hash) == 0, "[VoicePacket] : 'hash' field should be located at beginning of packet struct");

using VoicePacketContainer = Memory::ObjectContainer<VoicePacket>;
using VoicePacketContainerPtr = Memory::ObjectContainerPtr<VoicePacket>;
#define MakeVoicePacketContainer MakeObjectContainer(VoicePacket)
