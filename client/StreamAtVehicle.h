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
#include <game/CVehicle.h>

#include "StreamInfo.h"
#include "LocalStream.h"
#include "Channel.h"

class StreamAtVehicle : public LocalStream {

    StreamAtVehicle() = delete;
    StreamAtVehicle(const StreamAtVehicle&) = delete;
    StreamAtVehicle(StreamAtVehicle&&) = delete;
    StreamAtVehicle& operator=(const StreamAtVehicle&) = delete;
    StreamAtVehicle& operator=(StreamAtVehicle&&) = delete;

public:

    explicit StreamAtVehicle(PlayHandlerType&& playHandler, StopHandlerType&& stopHandler,
                             const std::string& name, const D3DCOLOR color,
                             const WORD vehicleId, const float distance)
        : LocalStream(BASS_SAMPLE_3D | BASS_SAMPLE_MUTEMAX,
                      std::move(playHandler), std::move(stopHandler),
                      StreamType::LocalStreamAtVehicle,
                      name, color, distance)
        , vehicleId(vehicleId) {}

    ~StreamAtVehicle() noexcept = default;

public:

    void Tick() noexcept override
    {
        this->LocalStream::Tick();

        const auto pNetGame = SAMP::pNetGame();
        if (!pNetGame) return;

        const auto pVehiclePool = pNetGame->GetVehiclePool();
        if (!pVehiclePool) return;

        const auto pVehicle = pVehiclePool->m_pGameObject[this->vehicleId];
        if (!pVehicle) return;

        const auto pVehicleMatrix = pVehicle->GetMatrix();
        if (!pVehicleMatrix) return;

        for (const auto& iChan : this->channels)
        {
            if (iChan->speaker != SV::kNonePlayer)
            {
                BASS_ChannelSet3DPosition(iChan->handle,
                    reinterpret_cast<BASS_3DVECTOR*>(&pVehicleMatrix->pos),
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

        const auto pVehiclePool = pNetGame->GetVehiclePool();
        if (!pVehiclePool) return;

        const auto pVehicle = pVehiclePool->m_pGameObject[this->vehicleId];
        if (!pVehicle) return;

        const auto pVehicleMatrix = pVehicle->GetMatrix();
        if (!pVehicleMatrix) return;

        BASS_ChannelSet3DPosition(channel.handle,
            reinterpret_cast<BASS_3DVECTOR*>(&pVehicleMatrix->pos),
            &kZeroVector, &kZeroVector);
    }

private:

    const WORD vehicleId { NULL };

};

using StreamAtVehiclePtr = std::shared_ptr<StreamAtVehicle>;
#define MakeStreamAtVehicle std::make_shared<StreamAtVehicle>
