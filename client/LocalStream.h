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

#include <audio/bass.h>

#include "Stream.h"
#include "StreamInfo.h"
#include "Channel.h"

class LocalStream : public Stream {

    LocalStream() = delete;
    LocalStream(const LocalStream&) = delete;
    LocalStream(LocalStream&&) = delete;
    LocalStream& operator=(const LocalStream&) = delete;
    LocalStream& operator=(LocalStream&&) = delete;

public:

    explicit LocalStream(const DWORD streamFlags, PlayHandlerType&& playHandler, StopHandlerType&& stopHandler,
                         const StreamType type, const std::string& name, const D3DCOLOR color, const float distance)
        : Stream(streamFlags, std::move(playHandler), std::move(stopHandler), type, name, color)
        , distance(distance) {}

    virtual ~LocalStream() noexcept = default;

public:

    void UpdateDistance(const float distance) noexcept
    {
        this->distance = distance;

        for (const auto& iChan : this->channels)
        {
            BASS_ChannelSet3DAttributes(iChan->handle, BASS_3DMODE_NORMAL,
                this->distance * 0.1f, this->distance, -1, -1, -1);
        }
    }

protected:

    virtual void ChannelCreationHandler(const Channel& channel) noexcept override
    {
        this->Stream::ChannelCreationHandler(channel);

        BASS_ChannelSet3DAttributes(channel.handle, BASS_3DMODE_NORMAL,
            this->distance * 0.1f, this->distance, -1, -1, -1);
    }

protected:

    float distance { 0.f };

};

using LocalStreamPtr = std::shared_ptr<LocalStream>;
#define MakeLocalStream std::make_shared<LocalStream>
