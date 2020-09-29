/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "DynamicStream.h"

DynamicStream::DynamicStream(const float distance, const uint32_t maxPlayers)
    : LocalStream(distance), maxPlayers(maxPlayers) {}

bool DynamicStream::AttachListener(uint16_t) noexcept { return false; }
bool DynamicStream::DetachListener(uint16_t) noexcept { return false; }

std::vector<uint16_t> DynamicStream::DetachAllListeners() noexcept
{
    return std::vector<uint16_t>();
}
