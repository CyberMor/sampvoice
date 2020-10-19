/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <memory>
#include <string>

#include <d3d9.h>

#include "Stream.h"
#include "StreamInfo.h"
#include "Channel.h"

class LocalStream : public Stream {

    LocalStream() = delete;
    LocalStream(const LocalStream&) = delete;
    LocalStream(LocalStream&&) = delete;
    LocalStream& operator=(const LocalStream&) = delete;
    LocalStream& operator=(LocalStream&&) = delete;

protected:

    explicit LocalStream(StreamType type, D3DCOLOR color,
                         std::string name, float distance) noexcept;

public:

    virtual ~LocalStream() noexcept = default;

public:

    void SetDistance(float distance) noexcept;

protected:

    void OnChannelCreate(const Channel& channel) noexcept override;

private:

    float distance;

};

using LocalStreamPtr = std::unique_ptr<LocalStream>;
