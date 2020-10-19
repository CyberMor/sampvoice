/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "PlayerStore.h"

#include <cassert>

#include <ysf/globals.h>

void PlayerStore::AddPlayerToStore(const uint16_t playerId, const uint8_t version, const bool microStatus)
{
    assert(playerId >= 0 && playerId < MAX_PLAYERS);

    if (const auto pPlayerInfo = new (std::nothrow) PlayerInfo(version, microStatus))
    {
        PlayerStore::playerMutex[playerId].lock();
        const auto pOldPlayerInfo = PlayerStore::playerInfo[playerId].exchange(pPlayerInfo, std::memory_order_acq_rel);
        PlayerStore::playerMutex[playerId].unlock();

        if (pOldPlayerInfo != nullptr)
        {
            for (const auto stream : pOldPlayerInfo->listenerStreams)
                stream->DetachListener(playerId);

            for (const auto stream : pOldPlayerInfo->speakerStreams)
                stream->DetachSpeaker(playerId);

            delete pOldPlayerInfo;
        }
    }
}

void PlayerStore::RemovePlayerFromStore(const uint16_t playerId)
{
    assert(playerId >= 0 && playerId < MAX_PLAYERS);

    PlayerStore::playerMutex[playerId].lock();
    const auto pPlayerInfo = PlayerStore::playerInfo[playerId].exchange(nullptr, std::memory_order_acq_rel);
    PlayerStore::playerMutex[playerId].unlock();

    if (pPlayerInfo != nullptr)
    {
        for (const auto stream : pPlayerInfo->listenerStreams)
            stream->DetachListener(playerId);

        for (const auto stream : pPlayerInfo->speakerStreams)
            stream->DetachSpeaker(playerId);

        delete pPlayerInfo;
    }
}

void PlayerStore::ClearStore()
{
    for (uint16_t playerId { 0 }; playerId < MAX_PLAYERS; ++playerId)
        PlayerStore::RemovePlayerFromStore(playerId);
}

bool PlayerStore::IsPlayerConnected(const uint16_t playerId) noexcept
{
    assert(pNetGame != nullptr);
    assert(pNetGame->pPlayerPool != nullptr);

    assert(playerId >= 0 && playerId < MAX_PLAYERS);

    return pNetGame->pPlayerPool->bIsPlayerConnected[playerId];
}

bool PlayerStore::IsPlayerHasPlugin(const uint16_t playerId) noexcept
{
    assert(playerId >= 0 && playerId < MAX_PLAYERS);

    return PlayerStore::playerInfo[playerId].load(std::memory_order_relaxed);
}

PlayerInfo* PlayerStore::RequestPlayerWithSharedAccess(const uint16_t playerId) noexcept
{
    assert(playerId >= 0 && playerId < MAX_PLAYERS);

    PlayerStore::playerMutex[playerId].lock_shared();

    return PlayerStore::playerInfo[playerId].load(std::memory_order_acquire);
}

void PlayerStore::ReleasePlayerWithSharedAccess(const uint16_t playerId) noexcept
{
    assert(playerId >= 0 && playerId < MAX_PLAYERS);

    PlayerStore::playerMutex[playerId].unlock_shared();
}

PlayerInfo* PlayerStore::RequestPlayerWithUniqueAccess(const uint16_t playerId) noexcept
{
    assert(playerId >= 0 && playerId < MAX_PLAYERS);

    PlayerStore::playerMutex[playerId].lock();

    return PlayerStore::playerInfo[playerId].load(std::memory_order_acquire);
}

void PlayerStore::ReleasePlayerWithUniqueAccess(const uint16_t playerId) noexcept
{
    assert(playerId >= 0 && playerId < MAX_PLAYERS);

    PlayerStore::playerMutex[playerId].unlock();
}

std::array<std::shared_mutex, MAX_PLAYERS> PlayerStore::playerMutex;
std::array<std::atomic<PlayerInfo*>, MAX_PLAYERS> PlayerStore::playerInfo {};
