/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "DynamicLocalStreamAtPoint.h"

#include <cassert>
#include <cstring>

#include <ysf/globals.h>
#include <util/memory.hpp>
#include <util/logger.h>

#include "ControlPacket.h"
#include "PlayerStore.h"
#include "Header.h"

DynamicLocalStreamAtPoint::DynamicLocalStreamAtPoint(
    const float distance, const uint32_t maxPlayers,
    const CVector& position, const uint32_t color,
    const std::string& name
)
    : LocalStream(distance)
    , DynamicStream(distance, maxPlayers)
    , PointStream(distance, position)
{
    assert(pNetGame != nullptr);
    assert(pNetGame->pPlayerPool != nullptr);

    const auto nameString = name.c_str();
    const auto nameLength = name.size() + 1;

    PackWrap(this->packetCreateStream, SV::ControlPacketType::createLPStream, sizeof(SV::CreateLPStreamPacket) + nameLength);

    PackGetStruct(&*this->packetCreateStream, SV::CreateLPStreamPacket)->stream = reinterpret_cast<uint32_t>(static_cast<Stream*>(this));
    std::memcpy(PackGetStruct(&*this->packetCreateStream, SV::CreateLPStreamPacket)->name, nameString, nameLength);
    PackGetStruct(&*this->packetCreateStream, SV::CreateLPStreamPacket)->distance = distance;
    PackGetStruct(&*this->packetCreateStream, SV::CreateLPStreamPacket)->position = position;
    PackGetStruct(&*this->packetCreateStream, SV::CreateLPStreamPacket)->color = color;

    PlayerSortList playerList;

    if (pNetGame->pPlayerPool->dwConnectedPlayers != 0)
    {
        const auto playerPoolSize = pNetGame->pPlayerPool->dwPlayerPoolSize;

        for (uint16_t iPlayerId { 0 }; iPlayerId <= playerPoolSize; ++iPlayerId)
        {
            const auto ipPlayer = pNetGame->pPlayerPool->pPlayer[iPlayerId];

            float distanceToPlayer;

            if (ipPlayer != nullptr && PlayerStore::IsPlayerHasPlugin(iPlayerId) &&
                (distanceToPlayer = (ipPlayer->vecPosition - position).Length()) <= distance)
            {
                playerList.emplace(distanceToPlayer, iPlayerId);
            }
        }
    }

    for (const auto& playerInfo : playerList)
    {
        if (this->attachedListenersCount >= maxPlayers)
            break;

        this->Stream::AttachListener(playerInfo.playerId);
    }
}

void DynamicLocalStreamAtPoint::Tick()
{
    assert(pNetGame != nullptr);
    assert(pNetGame->pPlayerPool != nullptr);

    PlayerSortList playerList;

    const CVector& streamPosition = PackGetStruct(&*this->packetCreateStream, SV::CreateLPStreamPacket)->position;
    const float streamDistance = PackGetStruct(&*this->packetStreamUpdateDistance, SV::UpdateLStreamDistancePacket)->distance;

    if (pNetGame->pPlayerPool->dwConnectedPlayers != 0)
    {
        const auto playerPoolSize = pNetGame->pPlayerPool->dwPlayerPoolSize;

        for (uint16_t iPlayerId { 0 }; iPlayerId <= playerPoolSize; ++iPlayerId)
        {
            const auto ipPlayer = pNetGame->pPlayerPool->pPlayer[iPlayerId];

            float distanceToPlayer;

            if (ipPlayer != nullptr && PlayerStore::IsPlayerHasPlugin(iPlayerId) &&
                (distanceToPlayer = (ipPlayer->vecPosition - streamPosition).Length()) <= streamDistance)
            {
                if (!this->HasListener(iPlayerId))
                {
                    playerList.emplace(distanceToPlayer, iPlayerId);
                }
            }
            else if (this->HasListener(iPlayerId))
            {
                this->Stream::DetachListener(iPlayerId);
            }
        }
    }

    for (const auto& playerInfo : playerList)
    {
        if (this->attachedListenersCount >= this->maxPlayers)
            break;

        this->Stream::AttachListener(playerInfo.playerId);
    }
}
