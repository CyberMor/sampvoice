/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cstdint>

#include <malloc.h>

#include <util/memory.hpp>

#pragma pack(push, 1)

struct ControlPacket
{
    uint16_t packet;
    uint16_t length;
    uint8_t  data[];

    uint32_t GetFullSize() const noexcept;
};

#pragma pack(pop)

using ControlPacketContainer = Memory::ObjectContainer<ControlPacket>;
using ControlPacketContainerPtr = Memory::ObjectContainerPtr<ControlPacket>;
#define MakeControlPacketContainer MakeObjectContainer(ControlPacket)

#define PackWrap(container, id, size) { (container) = MakeControlPacketContainer(size); (*(container))->packet = (id); (*(container))->length = (size); }
#define PackMalloc(pack, id, size) (((pack) = (ControlPacket*)(std::malloc(sizeof(ControlPacket) + (size)))) ? ((pack)->packet = (id), (pack)->length = (size), true) : false)
#define PackAlloca(pack, id, size) { (pack) = (ControlPacket*)(alloca(sizeof(ControlPacket) + (size))); (pack)->packet = (id); (pack)->length = (size); }
#define PackGetStruct(pack, type) ((type*)((pack)->data))
#define PackFree(pack) std::free(pack)
