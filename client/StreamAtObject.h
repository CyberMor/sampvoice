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
#include <samp/CNetGame.h>
#include <game/CObject.h>

#include "StreamInfo.h"
#include "LocalStream.h"
#include "Channel.h"

class StreamAtObject : public LocalStream {

    StreamAtObject() = delete;
    StreamAtObject(const StreamAtObject&) = delete;
    StreamAtObject(StreamAtObject&&) = delete;
    StreamAtObject& operator=(const StreamAtObject&) = delete;
    StreamAtObject& operator=(StreamAtObject&&) = delete;

public:

    explicit StreamAtObject(PlayHandlerType&& playHandler, StopHandlerType&& stopHandler,
                            const std::string& name, const D3DCOLOR color,
                            const WORD objectId, const float distance)
        : LocalStream(BASS_SAMPLE_3D | BASS_SAMPLE_MUTEMAX,
                      std::move(playHandler), std::move(stopHandler),
                      StreamType::LocalStreamAtObject,
                      name, color, distance)
        , objectId(objectId) {}

    ~StreamAtObject() noexcept = default;

public:

    void Tick() noexcept override
    {
        this->LocalStream::Tick();

        const auto pNetGame = SAMP::pNetGame();
        if (!pNetGame) return;

        const auto pObjectPool = pNetGame->GetObjectPool();
        if (!pObjectPool) return;

        const auto pObject = pObjectPool->m_pObject[this->objectId];
        if (!pObject) return;

        const auto pObjectEntity = pObject->m_pGameEntity;
        if (!pObjectEntity) return;

        const auto pObjectMatrix = pObjectEntity->GetMatrix();
        if (!pObjectMatrix) return;

        for (const auto& iChan : this->channels)
        {
            if (iChan->speaker != SV::NonePlayer)
            {
                BASS_ChannelSet3DPosition(iChan->handle,
                    reinterpret_cast<BASS_3DVECTOR*>(&pObjectMatrix->pos),
                    nullptr, nullptr);
            }
        }
    }

private:

    void ChannelCreationHandler(const Channel& channel) noexcept override
    {
        static const BASS_3DVECTOR kZeroVector { 0, 0, 0 };

        this->LocalStream::ChannelCreationHandler(channel);

        const auto pNetGame = SAMP::pNetGame();
        if (!pNetGame) return;

        const auto pObjectPool = pNetGame->GetObjectPool();
        if (!pObjectPool) return;

        const auto pObject = pObjectPool->m_pObject[this->objectId];
        if (!pObject) return;

        const auto pObjectEntity = pObject->m_pGameEntity;
        if (!pObjectEntity) return;

        const auto pObjectMatrix = pObjectEntity->GetMatrix();
        if (!pObjectMatrix) return;

        BASS_ChannelSet3DPosition(channel.handle,
            reinterpret_cast<BASS_3DVECTOR*>(&pObjectMatrix->pos),
            &kZeroVector, &kZeroVector);
    }

private:

    const WORD objectId { NULL };

};

using StreamAtObjectPtr = std::shared_ptr<StreamAtObject>;
#define MakeStreamAtObject std::make_shared<StreamAtObject>
