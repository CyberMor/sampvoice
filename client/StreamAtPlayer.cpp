#include "StreamAtPlayer.h"

#include <audio/bass.h>
#include <samp/CNetGame.h>
#include <game/CPed.h>

#include "StreamInfo.h"

StreamAtPlayer::StreamAtPlayer(const D3DCOLOR color, std::string name, const float distance, const WORD player_id) noexcept
    : LocalStream { StreamType::LocalStreamAtPlayer, color, std::move(name), distance }
    , _player_id  { player_id }
{}

void StreamAtPlayer::Tick() noexcept
{
    LocalStream::Tick();

    if (const auto net_game = SAMP::pNetGame(); net_game != nullptr)
    {
        if (const auto player_pool = net_game->GetPlayerPool(); player_pool != nullptr)
        {
            if (const auto player = player_pool->GetPlayer(player_id); player != nullptr)
            {
                if (const auto player_ped = player->m_pPed; player_ped != nullptr)
                {
                    if (const auto player_game_ped = player_ped->m_pGamePed; player_game_ped != nullptr)
                    {
                        if (const auto player_matrix = player_game_ped->GetMatrix(); player_matrix != nullptr)
                        {
                            for (const auto& channel : GetChannels())
                            {
                                if (channel.HasSpeaker())
                                {
                                    BASS_ChannelSet3DPosition(channel.GetHandle(),
                                        reinterpret_cast<BASS_3DVECTOR*>(&player_matrix->pos),
                                        nullptr, nullptr);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void StreamAtPlayer::OnChannelCreate(const Channel& channel) noexcept
{
    static const BASS_3DVECTOR kZeroVector { 0, 0, 0 };

    LocalStream::OnChannelCreate(channel);

    if (const auto net_game = SAMP::pNetGame(); net_game != nullptr)
    {
        if (const auto player_pool = net_game->GetPlayerPool(); player_pool != nullptr)
        {
            if (const auto player = player_pool->GetPlayer(_player_id); player != nullptr)
            {
                if (const auto player_ped = player->m_pPed; player_ped != nullptr)
                {
                    if (const auto player_game_ped = player_ped->m_pGamePed; player_game_ped != nullptr)
                    {
                        if (const auto player_matrix = player_game_ped->GetMatrix(); player_matrix != nullptr)
                        {
                            BASS_ChannelSet3DPosition(channel.GetHandle(),
                                reinterpret_cast<BASS_3DVECTOR*>(&player_matrix->pos),
                                &kZeroVector, &kZeroVector);
                        }
                    }
                }
            }
        }
    }
}
