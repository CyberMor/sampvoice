/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "VoicePacket.h"

static uint32_t CalcCrc32cHash(const uint8_t* buffer, uint32_t length, uint32_t crc = 0) noexcept
{
    crc = ~crc;

    while (length--)
    {
        crc ^= *buffer++;

        for (int k = 0; k < 8; ++k) crc = crc & 1 ? (crc >> 1) ^ 0x82f63b78 : crc >> 1;
    }

    return ~crc;
}

uint32_t VoicePacket::GetFullSize() const noexcept
{
    return sizeof(*this) + this->length;
}

bool VoicePacket::CheckHeader() const noexcept
{
    return this->hash == CalcCrc32cHash(
        (uint8_t*)(this) + sizeof(this->hash),
        sizeof(*this) - sizeof(this->hash)
    );
}

void VoicePacket::CalcHash() noexcept
{
    this->hash = CalcCrc32cHash(
        (uint8_t*)(this) + sizeof(this->hash),
        sizeof(*this) - sizeof(this->hash)
    );
}
