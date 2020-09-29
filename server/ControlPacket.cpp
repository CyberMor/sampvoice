/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "ControlPacket.h"

uint32_t ControlPacket::GetFullSize() const noexcept
{
    return sizeof(*this) + this->length;
}
