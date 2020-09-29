/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <ysf/utils/cvector.h>

#include "ControlPacket.h"
#include "LocalStream.h"

class PointStream : public virtual LocalStream {

    PointStream() = delete;
    PointStream(const PointStream&) = delete;
    PointStream(PointStream&&) = delete;
    PointStream& operator=(const PointStream&) = delete;
    PointStream& operator=(PointStream&&) = delete;

protected:

    explicit PointStream(float distance, const CVector& position);

public:

    virtual ~PointStream() noexcept = default;

public:

    void UpdatePosition(const CVector& position);

protected:

    ControlPacketContainerPtr packetStreamUpdatePosition { nullptr };

};
