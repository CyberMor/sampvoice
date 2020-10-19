/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "LocalStream.h"

#include <cassert>

#include <ysf/globals.h>

#include "Network.h"
#include "PlayerStore.h"
#include "Header.h"

LocalStream::LocalStream(const float distance)
{
    PackWrap(this->packetStreamUpdateDistance, SV::ControlPacketType::updateLStreamDistance, sizeof(SV::UpdateLStreamDistancePacket));

    PackGetStruct(&*this->packetStreamUpdateDistance, SV::UpdateLStreamDistancePacket)->stream = reinterpret_cast<uint32_t>(static_cast<Stream*>(this));
    PackGetStruct(&*this->packetStreamUpdateDistance, SV::UpdateLStreamDistancePacket)->distance = distance;
}

void LocalStream::UpdateDistance(const float distance)
{
    assert(pNetGame != nullptr);
    assert(pNetGame->pPlayerPool != nullptr);

    PackGetStruct(&*this->packetStreamUpdateDistance, SV::UpdateLStreamDistancePacket)->distance = distance;

    if (pNetGame->pPlayerPool->dwConnectedPlayers != 0)
    {
        const auto playerPoolSize = pNetGame->pPlayerPool->dwPlayerPoolSize;

        for (uint16_t iPlayerId { 0 }; iPlayerId <= playerPoolSize; ++iPlayerId)
        {
            if (this->HasListener(iPlayerId) && PlayerStore::IsPlayerConnected(iPlayerId))
                Network::SendControlPacket(iPlayerId, *&*this->packetStreamUpdateDistance);
        }
    }
}
