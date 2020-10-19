#include "StreamAtPlayer.h"

#include <audio/bass.h>
#include <samp/CNetGame.h>
#include <game/CPed.h>

#include "StreamInfo.h"

StreamAtPlayer::StreamAtPlayer(const D3DCOLOR color, std::string name,
                               const float distance, const WORD playerId) noexcept
    : LocalStream(StreamType::LocalStreamAtPlayer, color, std::move(name), distance)
    , playerId(playerId)
{}

void StreamAtPlayer::Tick() noexcept
{
    this->LocalStream::Tick();

    const auto pNetGame = SAMP::pNetGame();
    if (!pNetGame) return;

    const auto pPlayerPool = pNetGame->GetPlayerPool();
    if (!pPlayerPool) return;

    const auto pPlayer = pPlayerPool->GetPlayer(this->playerId);
    if (!pPlayer) return;

    const auto pPlayerPed = pPlayer->m_pPed;
    if (!pPlayerPed) return;

    const auto pPlayerGamePed = pPlayerPed->m_pGamePed;
    if (!pPlayerGamePed) return;

    const auto pPlayerMatrix = pPlayerGamePed->GetMatrix();
    if (!pPlayerMatrix) return;

    for (const auto& channel : this->GetChannels())
    {
        if (channel->HasSpeaker())
        {
            BASS_ChannelSet3DPosition(channel->GetHandle(),
                reinterpret_cast<BASS_3DVECTOR*>(&pPlayerMatrix->pos),
                nullptr, nullptr);
        }
    }
}

void StreamAtPlayer::OnChannelCreate(const Channel& channel) noexcept
{
    static const BASS_3DVECTOR kZeroVector { 0, 0, 0 };

    this->LocalStream::OnChannelCreate(channel);

    const auto pNetGame = SAMP::pNetGame();
    if (!pNetGame) return;

    const auto pPlayerPool = pNetGame->GetPlayerPool();
    if (!pPlayerPool) return;

    const auto pPlayer = pPlayerPool->GetPlayer(this->playerId);
    if (!pPlayer) return;

    const auto pPlayerPed = pPlayer->m_pPed;
    if (!pPlayerPed) return;

    const auto pPlayerGamePed = pPlayerPed->m_pGamePed;
    if (!pPlayerGamePed) return;

    const auto pPlayerMatrix = pPlayerGamePed->GetMatrix();
    if (!pPlayerMatrix) return;

    BASS_ChannelSet3DPosition(channel.GetHandle(),
        reinterpret_cast<BASS_3DVECTOR*>(&pPlayerMatrix->pos),
        &kZeroVector, &kZeroVector);
}
