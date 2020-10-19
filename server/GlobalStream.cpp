/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "GlobalStream.h"

#include <cstring>

#include "ControlPacket.h"
#include "Header.h"

GlobalStream::GlobalStream(const uint32_t color, const std::string& name)
{
    const auto nameString = name.c_str();
    const auto nameLength = name.size() + 1;

    PackWrap(this->packetCreateStream, SV::ControlPacketType::createGStream, sizeof(SV::CreateGStreamPacket) + nameLength);

    PackGetStruct(&*this->packetCreateStream, SV::CreateGStreamPacket)->stream = reinterpret_cast<uint32_t>(static_cast<Stream*>(this));
    std::memcpy(PackGetStruct(&*this->packetCreateStream, SV::CreateGStreamPacket)->name, nameString, nameLength);
    PackGetStruct(&*this->packetCreateStream, SV::CreateGStreamPacket)->color = color;
}
