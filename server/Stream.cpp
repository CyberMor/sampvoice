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

Stream::~Stream() noexcept
{
    for (const auto& deleteCallback : this->deleteCallbacks)
    {
        if (deleteCallback != nullptr) deleteCallback(this);
    }
}

void Stream::SendVoicePacket(VoicePacket& voicePacket) const
{
    assert(pNetGame != nullptr);
    assert(pNetGame->pPlayerPool != nullptr);

    assert(voicePacket.sender >= 0 && voicePacket.sender < MAX_PLAYERS);

    if (!this->HasSpeaker(voicePacket.sender))
        return;

    voicePacket.stream = reinterpret_cast<uint32_t>(this);
    voicePacket.CalcHash();

    if (pNetGame->pPlayerPool->dwConnectedPlayers != 0)
    {
        const auto playerPoolSize = pNetGame->pPlayerPool->dwPlayerPoolSize;

        for (uint16_t iPlayerId { 0 }; iPlayerId <= playerPoolSize; ++iPlayerId)
        {
            if (this->HasListener(iPlayerId) && PlayerStore::IsPlayerConnected(iPlayerId) && iPlayerId != voicePacket.sender)
                Network::SendVoicePacket(iPlayerId, voicePacket);
        }
    }
}

void Stream::SendControlPacket(ControlPacket& controlPacket) const
{
    assert(pNetGame != nullptr);
    assert(pNetGame->pPlayerPool != nullptr);

    if (pNetGame->pPlayerPool->dwConnectedPlayers != 0)
    {
        const auto playerPoolSize = pNetGame->pPlayerPool->dwPlayerPoolSize;

        for (uint16_t iPlayerId { 0 }; iPlayerId <= playerPoolSize; ++iPlayerId)
        {
            if (this->HasListener(iPlayerId) && PlayerStore::IsPlayerConnected(iPlayerId))
                Network::SendControlPacket(iPlayerId, controlPacket);
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

    for (const auto& playerCallback : this->playerCallbacks)
    {
        if (playerCallback != nullptr) playerCallback(this, playerId);
    }

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

    for (uint16_t iPlayerId { 0 }; iPlayerId < MAX_PLAYERS; ++iPlayerId)
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

    for (uint16_t iPlayerId { 0 }; iPlayerId < MAX_PLAYERS; ++iPlayerId)
    {
        if (this->attachedSpeakers[iPlayerId].exchange(false, std::memory_order_relaxed))
            detachedSpeakers.emplace_back(iPlayerId);
    }

    this->attachedSpeakersCount = 0;

    return detachedSpeakers;
}

namespace
{
    const std::map<uint8_t, float> kDefaultValues =
    {
        { SV::ParameterType::frequency, 0.f },
        { SV::ParameterType::volume,    1.f },
        { SV::ParameterType::panning,   0.f },
        { SV::ParameterType::eaxmix,   -1.f },
        { SV::ParameterType::src,       1.f }
    };
}

void Stream::SetParameter(const uint8_t parameter, const float value) noexcept
{
    const auto valueIter = kDefaultValues.find(parameter);
    if (valueIter == kDefaultValues.end()) return;

    const auto iter = this->parameters.try_emplace(parameter, this, parameter, value);
    if (!iter.second) iter.first->second.Set(value);
}

void Stream::ResetParameter(const uint8_t parameter) noexcept
{
    assert(pNetGame != nullptr);
    assert(pNetGame->pPlayerPool != nullptr);

    const auto valueIter = kDefaultValues.find(parameter);
    if (valueIter == kDefaultValues.end()) return;

    const auto iter = this->parameters.find(parameter);
    if (iter != this->parameters.end())
    {
        iter->second.Set(valueIter->second);

        if (pNetGame->pPlayerPool->dwConnectedPlayers != 0)
        {
            const auto playerPoolSize = pNetGame->pPlayerPool->dwPlayerPoolSize;

            for (uint16_t iPlayerId { 0 }; iPlayerId <= playerPoolSize; ++iPlayerId)
            {
                if (this->HasListener(iPlayerId) && PlayerStore::IsPlayerConnected(iPlayerId))
                    iter->second.ApplyForPlayer(iPlayerId);
            }
        }

        this->parameters.erase(iter);
    }
}

bool Stream::HasParameter(const uint8_t parameter) const noexcept
{
    return this->parameters.find(parameter) != this->parameters.end();
}

float Stream::GetParameter(const uint8_t parameter) noexcept
{
    const auto valueIter = kDefaultValues.find(parameter);
    if (valueIter == kDefaultValues.end()) return -1.f;

    const auto iter = this->parameters.find(parameter);
    return iter != this->parameters.end() ? iter->second.Get() : valueIter->second;
}

void Stream::SlideParameterFromTo(const uint8_t parameter, const float startValue, const float endValue, const uint32_t time) noexcept
{
    const auto valueIter = kDefaultValues.find(parameter);
    if (valueIter == kDefaultValues.end()) return;

    const auto iter = this->parameters.try_emplace(parameter, this, parameter, valueIter->second);
    iter.first->second.SlideFromTo(startValue, endValue, time);
}

void Stream::SlideParameterTo(const uint8_t parameter, const float endValue, const uint32_t time) noexcept
{
    const auto valueIter = kDefaultValues.find(parameter);
    if (valueIter == kDefaultValues.end()) return;

    const auto iter = this->parameters.try_emplace(parameter, this, parameter, valueIter->second);
    iter.first->second.SlideTo(endValue, time);
}

void Stream::SlideParameter(const uint8_t parameter, const float deltaValue, const uint32_t time) noexcept
{
    const auto valueIter = kDefaultValues.find(parameter);
    if (valueIter == kDefaultValues.end()) return;

    const auto iter = this->parameters.try_emplace(parameter, this, parameter, valueIter->second);
    iter.first->second.Slide(deltaValue, time);
}

std::size_t Stream::AddPlayerCallback(PlayerCallback playerCallback) noexcept
{
    for (std::size_t i { 0 }; i < this->playerCallbacks.size(); ++i)
    {
        if (this->playerCallbacks[i] == nullptr)
        {
            this->playerCallbacks[i] = std::move(playerCallback);
            return i;
        }
    }

    this->playerCallbacks.emplace_back(std::move(playerCallback));
    return this->playerCallbacks.size() - 1;
}

std::size_t Stream::AddDeleteCallback(DeleteCallback deleteCallback) noexcept
{
    for (std::size_t i { 0 }; i < this->deleteCallbacks.size(); ++i)
    {
        if (this->deleteCallbacks[i] == nullptr)
        {
            this->deleteCallbacks[i] = std::move(deleteCallback);
            return i;
        }
    }

    this->deleteCallbacks.emplace_back(std::move(deleteCallback));
    return this->deleteCallbacks.size() - 1;
}

void Stream::RemovePlayerCallback(const std::size_t callback) noexcept
{
    if (callback >= this->playerCallbacks.size())
        return;

    this->playerCallbacks[callback] = nullptr;
}

void Stream::RemoveDeleteCallback(const std::size_t callback) noexcept
{
    if (callback >= this->deleteCallbacks.size())
        return;

    this->deleteCallbacks[callback] = nullptr;
}
