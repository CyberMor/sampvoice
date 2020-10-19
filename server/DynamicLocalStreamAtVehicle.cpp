/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "DynamicLocalStreamAtVehicle.h"

#include <cassert>
#include <cstring>

#include <ysf/globals.h>
#include <util/memory.hpp>
#include <util/logger.h>

#include "ControlPacket.h"
#include "PlayerStore.h"
#include "Header.h"

DynamicLocalStreamAtVehicle::DynamicLocalStreamAtVehicle(
    const float distance, const uint32_t maxPlayers,
    const uint16_t vehicleId, const uint32_t color,
    const std::string& name
)
    : LocalStream(distance)
    , DynamicStream(distance, maxPlayers)
{
    assert(pNetGame != nullptr);
    assert(pNetGame->pPlayerPool != nullptr);
    assert(pNetGame->pVehiclePool != nullptr);

    const auto nameString = name.c_str();
    const auto nameLength = name.size() + 1;

    PackWrap(this->packetCreateStream, SV::ControlPacketType::createLStreamAtVehicle, sizeof(SV::CreateLStreamAtPacket) + nameLength);

    PackGetStruct(&*this->packetCreateStream, SV::CreateLStreamAtPacket)->stream = reinterpret_cast<uint32_t>(static_cast<Stream*>(this));
    std::memcpy(PackGetStruct(&*this->packetCreateStream, SV::CreateLStreamAtPacket)->name, nameString, nameLength);
    PackGetStruct(&*this->packetCreateStream, SV::CreateLStreamAtPacket)->distance = distance;
    PackGetStruct(&*this->packetCreateStream, SV::CreateLStreamAtPacket)->target = vehicleId;
    PackGetStruct(&*this->packetCreateStream, SV::CreateLStreamAtPacket)->color = color;

    if (pNetGame->pVehiclePool->pVehicle[vehicleId] != nullptr)
    {
        PlayerSortList playerList;

        const CVector& streamPosition = pNetGame->pVehiclePool->pVehicle[vehicleId]->vecPosition;

        if (pNetGame->pPlayerPool->dwConnectedPlayers != 0)
        {
            const auto playerPoolSize = pNetGame->pPlayerPool->dwPlayerPoolSize;

            for (uint16_t iPlayerId { 0 }; iPlayerId <= playerPoolSize; ++iPlayerId)
            {
                const auto ipPlayer = pNetGame->pPlayerPool->pPlayer[iPlayerId];

                float distanceToPlayer;

                if (ipPlayer != nullptr && PlayerStore::IsPlayerHasPlugin(iPlayerId) && ipPlayer->byteVehicleStreamedIn[vehicleId] &&
                    (distanceToPlayer = (ipPlayer->vecPosition - streamPosition).Length()) <= distance)
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
}

void DynamicLocalStreamAtVehicle::Tick()
{
    assert(pNetGame != nullptr);
    assert(pNetGame->pPlayerPool != nullptr);
    assert(pNetGame->pVehiclePool != nullptr);

    const auto vehicleId = PackGetStruct(&*this->packetCreateStream, SV::CreateLStreamAtPacket)->target;

    if (pNetGame->pVehiclePool->pVehicle[vehicleId] != nullptr)
    {
        PlayerSortList playerList;

        const CVector& streamPosition = pNetGame->pVehiclePool->pVehicle[vehicleId]->vecPosition;
        const float streamDistance = PackGetStruct(&*this->packetStreamUpdateDistance, SV::UpdateLStreamDistancePacket)->distance;

        if (pNetGame->pPlayerPool->dwConnectedPlayers != 0)
        {
            const auto playerPoolSize = pNetGame->pPlayerPool->dwPlayerPoolSize;

            for (uint16_t iPlayerId { 0 }; iPlayerId <= playerPoolSize; ++iPlayerId)
            {
                const auto ipPlayer = pNetGame->pPlayerPool->pPlayer[iPlayerId];

                float distanceToPlayer;

                if (ipPlayer != nullptr && PlayerStore::IsPlayerHasPlugin(iPlayerId) && ipPlayer->byteVehicleStreamedIn[vehicleId] &&
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
}
