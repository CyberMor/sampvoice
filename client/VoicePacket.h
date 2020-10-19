/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cstddef>

#include <Windows.h>

#include <util/Memory.hpp>

#pragma pack(push, 1)

struct VoicePacket
{
    UINT32 hash;
    UINT32 svrkey;
    UINT32 packet;
    UINT32 stream;
    UINT16 sender;
    UINT16 length;
    UINT32 packid;
    UINT8 data[];

    DWORD GetFullSize() const noexcept;
    bool CheckHeader() const noexcept;
    void CalcHash() noexcept;
};

#pragma pack(pop)

static_assert(offsetof(VoicePacket, hash) == 0, "[VoicePacket] : 'hash' "
    "field should be located at beginning of packet struct");

using VoicePacketContainer = Memory::ObjectContainer<VoicePacket>;
using VoicePacketContainerPtr = Memory::ObjectContainerPtr<VoicePacket>;
#define MakeVoicePacketContainer MakeObjectContainer(VoicePacket)
