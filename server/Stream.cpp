/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Stream.h"

#include <cassert>

#include <ysf/globals.h>

#include "Network.h"
#include "PlayerStore.h"
#include "Header.h"

Stream::Stream()
{
    PackWrap(this->packetDeleteStream, SV::ControlPacketType::deleteStream, sizeof(SV::DeleteStreamPacket));

    PackGetStruct(&*this->packetDeleteStream, SV::DeleteStreamPacket)->stream = reinterpret_cast<uint32_t>(this);
}

void Stream::PushVoicePacket(VoicePacket& voicePacket) const
{
    assert(pNetGame);
    assert(pNetGame->pPlayerPool);

    assert(voicePacket.sender >= 0 && voicePacket.sender < MAX_PLAYERS);

    if (!this->HasSpeaker(voicePacket.sender)) return;

    voicePacket.stream = reinterpret_cast<uint32_t>(this);
    voicePacket.CalcHash();

    if (pNetGame->pPlayerPool->dwConnectedPlayers)
    {
        const auto playerPoolSize = pNetGame->pPlayerPool->dwPlayerPoolSize;

        for (uint16_t iPlayerId = 0; iPlayerId <= playerPoolSize; ++iPlayerId)
        {
            if (this->HasListener(iPlayerId) &&
                PlayerStore::IsPlayerConnected(iPlayerId) &&
                iPlayerId != voicePacket.sender)
            {
                Network::SendVoicePacket(iPlayerId, voicePacket);
            }
        }
    }
}

bool Stream::AttachListener(const uint16_t playerId)
{
    assert(playerId < MAX_PLAYERS);

    if (!PlayerStore::IsPlayerHasPlugin(playerId)) return false;
    if (this->attachedListeners[playerId].exchange(true, std::memory_order_relaxed))
        return false;

    Network::SendControlPacket(playerId, *&*this->packetCreateStream);

    ++this->attachedListenersCount;

    return true;
}

bool Stream::HasListener(const uint16_t playerId) const noexcept
{
    assert(playerId < MAX_PLAYERS);

    return this->attachedListeners[playerId].load(std::memory_order_relaxed);
}

bool Stream::DetachListener(const uint16_t playerId)
{
    assert(playerId < MAX_PLAYERS);

    if (!this->attachedListeners[playerId].exchange(false, std::memory_order_relaxed))
        return false;

    if (PlayerStore::IsPlayerConnected(playerId) && this->packetDeleteStream)
        Network::SendControlPacket(playerId, *&*this->packetDeleteStream);

    --this->attachedListenersCount;

    return true;
}

std::vector<uint16_t> Stream::DetachAllListeners()
{
    std::vector<uint16_t> detachedListeners;

    detachedListeners.reserve(this->attachedListenersCount);

    for (uint16_t iPlayerId = 0; iPlayerId < MAX_PLAYERS; ++iPlayerId)
    {
        if (this->attachedListeners[iPlayerId].exchange(false, std::memory_order_relaxed))
        {
            if (PlayerStore::IsPlayerConnected(iPlayerId) && this->packetDeleteStream)
                Network::SendControlPacket(iPlayerId, *&*this->packetDeleteStream);

            detachedListeners.emplace_back(iPlayerId);
        }
    }

    this->attachedListenersCount = 0;

    return detachedListeners;
}

bool Stream::AttachSpeaker(const uint16_t playerId) noexcept
{
    assert(playerId < MAX_PLAYERS);

    if (!PlayerStore::IsPlayerHasPlugin(playerId)) return false;
    if (this->attachedSpeakers[playerId].exchange(true, std::memory_order_relaxed))
        return false;

    ++this->attachedSpeakersCount;

    return true;
}

bool Stream::HasSpeaker(const uint16_t playerId) const noexcept
{
    assert(playerId < MAX_PLAYERS);

    return this->attachedSpeakers[playerId].load(std::memory_order_relaxed);
}

bool Stream::DetachSpeaker(const uint16_t playerId) noexcept
{
    assert(playerId < MAX_PLAYERS);

    if (!this->attachedSpeakers[playerId].exchange(false, std::memory_order_relaxed))
        return false;

    --this->attachedSpeakersCount;

    return true;
}

std::vector<uint16_t> Stream::DetachAllSpeakers()
{
    std::vector<uint16_t> detachedSpeakers;

    detachedSpeakers.reserve(this->attachedSpeakersCount);

    for (uint16_t iPlayerId = 0; iPlayerId < MAX_PLAYERS; ++iPlayerId)
    {
        if (this->attachedSpeakers[iPlayerId].exchange(false, std::memory_order_relaxed))
            detachedSpeakers.emplace_back(iPlayerId);
    }

    this->attachedSpeakersCount = 0;

    return detachedSpeakers;
}
