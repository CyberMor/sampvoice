#include "StreamAtVehicle.h"

#include <audio/bass.h>
#include <samp/CNetGame.h>
#include <game/CVehicle.h>

#include "StreamInfo.h"

StreamAtVehicle::StreamAtVehicle(const D3DCOLOR color, std::string name, const float distance, const WORD vehicle_id) noexcept
    : LocalStream { StreamType::LocalStreamAtVehicle, color, std::move(name), distance }
    , _vehicle_id { vehicle_id }
{}

void StreamAtVehicle::Tick() noexcept
{
    LocalStream::Tick();

    if (const auto net_game = SAMP::pNetGame(); net_game != nullptr)
    {
        if (const auto vehicle_pool = net_game->GetVehiclePool(); vehicle_pool != nullptr)
        {
            if (const auto vehicle = vehicle_pool->m_pGameObject[_vehicle_id]; vehicle != nullptr)
            {
                if (const auto vehicle_matrix = vehicle->GetMatrix(); vehicle_matrix != nullptr)
                {
                    for (const auto& channel : GetChannels())
                    {
                        if (channel.HasSpeaker())
                        {
                            BASS_ChannelSet3DPosition(channel.GetHandle(),
                                reinterpret_cast<BASS_3DVECTOR*>(&vehicle_matrix->pos),
                                nullptr, nullptr);
                        }
                    }
                }
            }
        }
    }
}

void StreamAtVehicle::OnChannelCreate(const Channel& channel) noexcept
{
    static const BASS_3DVECTOR kZeroVector { 0, 0, 0 };

    LocalStream::OnChannelCreate(channel);

    if (const auto net_game = SAMP::pNetGame(); net_game != nullptr)
    {
        if (const auto vehicle_pool = net_game->GetVehiclePool(); vehicle_pool != nullptr)
        {
            if (const auto vehicle = vehicle_pool->m_pGameObject[_vehicle_id]; vehicle != nullptr)
            {
                if (const auto vehicle_matrix = vehicle->GetMatrix(); vehicle_matrix != nullptr)
                {
                    BASS_ChannelSet3DPosition(channel.GetHandle(),
                        reinterpret_cast<BASS_3DVECTOR*>(&vehicle_matrix->pos),
                        &kZeroVector, &kZeroVector);
                }
            }
        }
    }
}
