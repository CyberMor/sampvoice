/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <string>

#include <d3d9.h>

#include "Stream.h"
#include "StreamInfo.h"
#include "Channel.h"

struct LocalStream : public Stream {

    LocalStream() = delete;
    virtual ~LocalStream() noexcept = default;
    LocalStream(const LocalStream&) = delete;
    LocalStream(LocalStream&&) = delete;
    LocalStream& operator=(const LocalStream&) = delete;
    LocalStream& operator=(LocalStream&&) = delete;

protected:

    LocalStream(StreamType type, D3DCOLOR color, std::string name, float distance) noexcept;

public:

    void SetDistance(float distance) noexcept;

protected:

    virtual void OnChannelCreate(const Channel& channel) noexcept override;

private:

    float _distance;

};
