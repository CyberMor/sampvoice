#include "StreamAtVehicle.h"

#include <audio/bass.h>
#include <samp/CNetGame.h>
#include <game/CVehicle.h>

#include "StreamInfo.h"

StreamAtVehicle::StreamAtVehicle(const D3DCOLOR color, std::string name,
                                 const float distance, const WORD vehicleId) noexcept
    : LocalStream(StreamType::LocalStreamAtVehicle, color, std::move(name), distance)
    , vehicleId(vehicleId)
{}

void StreamAtVehicle::Tick() noexcept
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

    for (const auto& channel : this->GetChannels())
    {
        if (channel->HasSpeaker())
        {
            BASS_ChannelSet3DPosition(channel->GetHandle(),
                reinterpret_cast<BASS_3DVECTOR*>(&pVehicleMatrix->pos),
                nullptr, nullptr);
        }
    }
}

void StreamAtVehicle::OnChannelCreate(const Channel& channel) noexcept
{
    static const BASS_3DVECTOR kZeroVector { 0, 0, 0 };

    this->LocalStream::OnChannelCreate(channel);

    const auto pNetGame = SAMP::pNetGame();
    if (!pNetGame) return;

    const auto pVehiclePool = pNetGame->GetVehiclePool();
    if (!pVehiclePool) return;

    const auto pVehicle = pVehiclePool->m_pGameObject[this->vehicleId];
    if (!pVehicle) return;

    const auto pVehicleMatrix = pVehicle->GetMatrix();
    if (!pVehicleMatrix) return;

    BASS_ChannelSet3DPosition(channel.GetHandle(),
        reinterpret_cast<BASS_3DVECTOR*>(&pVehicleMatrix->pos),
        &kZeroVector, &kZeroVector);
}
