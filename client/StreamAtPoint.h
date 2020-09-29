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
#include <game/CVector.h>

#include "StreamInfo.h"
#include "LocalStream.h"
#include "Channel.h"

class StreamAtPoint : public LocalStream {

    StreamAtPoint() = delete;
    StreamAtPoint(const StreamAtPoint&) = delete;
    StreamAtPoint(StreamAtPoint&&) = delete;
    StreamAtPoint& operator=(const StreamAtPoint&) = delete;
    StreamAtPoint& operator=(StreamAtPoint&&) = delete;

public:

    explicit StreamAtPoint(PlayHandlerType&& playHandler, StopHandlerType&& stopHandler,
                           const std::string& name, const D3DCOLOR color,
                           const CVector& position, const float distance)
        : LocalStream(BASS_SAMPLE_3D | BASS_SAMPLE_MUTEMAX,
                      std::move(playHandler), std::move(stopHandler),
                      StreamType::LocalStreamAtPoint,
                      name, color, distance)
        , position(position) {}

    ~StreamAtPoint() noexcept = default;

public:

    void UpdatePosition(const CVector& position) noexcept
    {
        this->position = position;

        for (const auto& iChan : this->channels)
        {
            BASS_ChannelSet3DPosition(iChan->handle,
                reinterpret_cast<BASS_3DVECTOR*>(&this->position),
                nullptr, nullptr);
        }
    }

private:

    void ChannelCreationHandler(const Channel& channel) noexcept override
    {
        static const BASS_3DVECTOR kZeroVector { 0, 0, 0 };

        this->LocalStream::ChannelCreationHandler(channel);

        BASS_ChannelSet3DPosition(channel.handle,
            reinterpret_cast<BASS_3DVECTOR*>(&this->position),
            &kZeroVector, &kZeroVector);
    }

private:

    CVector position { 0, 0, 0 };

};

using StreamAtPointPtr = std::shared_ptr<StreamAtPoint>;
#define MakeStreamAtPoint std::make_shared<StreamAtPoint>
