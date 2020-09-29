/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include "ControlPacket.h"
#include "Stream.h"

class LocalStream : public Stream {

    LocalStream() = delete;
    LocalStream(const LocalStream&) = delete;
    LocalStream(LocalStream&&) = delete;
    LocalStream& operator=(const LocalStream&) = delete;
    LocalStream& operator=(LocalStream&&) = delete;

protected:

    explicit LocalStream(float distance);

public:

    virtual ~LocalStream() noexcept = default;

public:

    void UpdateDistance(float distance);

protected:

    ControlPacketContainerPtr packetStreamUpdateDistance { nullptr };

};
