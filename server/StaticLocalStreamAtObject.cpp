/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "StaticLocalStreamAtObject.h"

#include <cstring>

#include "ControlPacket.h"
#include "Header.h"

StaticLocalStreamAtObject::StaticLocalStreamAtObject(
    const float distance, const uint16_t objectId,
    const uint32_t color, const std::string& name
)
    : LocalStream(distance)
{
    const auto nameString = name.c_str();
    const auto nameLength = name.size() + 1;

    PackWrap(this->packetCreateStream, SV::ControlPacketType::createLStreamAtObject, sizeof(SV::CreateLStreamAtPacket) + nameLength);

    PackGetStruct(&*this->packetCreateStream, SV::CreateLStreamAtPacket)->stream = reinterpret_cast<uint32_t>(static_cast<Stream*>(this));
    std::memcpy(PackGetStruct(&*this->packetCreateStream, SV::CreateLStreamAtPacket)->name, nameString, nameLength);
    PackGetStruct(&*this->packetCreateStream, SV::CreateLStreamAtPacket)->distance = distance;
    PackGetStruct(&*this->packetCreateStream, SV::CreateLStreamAtPacket)->target = objectId;
    PackGetStruct(&*this->packetCreateStream, SV::CreateLStreamAtPacket)->color = color;
}
