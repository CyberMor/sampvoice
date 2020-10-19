/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "PointStream.h"

#include <cassert>

#include <ysf/globals.h>

#include "Network.h"
#include "PlayerStore.h"
#include "Header.h"

PointStream::PointStream(const float distance, const CVector& position) : LocalStream(distance)
{
    PackWrap(this->packetStreamUpdatePosition, SV::ControlPacketType::updateLPStreamPosition, sizeof(SV::UpdateLPStreamPositionPacket));

    PackGetStruct(&*this->packetStreamUpdatePosition, SV::UpdateLPStreamPositionPacket)->stream = reinterpret_cast<uint32_t>(static_cast<Stream*>(this));
    PackGetStruct(&*this->packetStreamUpdatePosition, SV::UpdateLPStreamPositionPacket)->position = position;
}

void PointStream::UpdatePosition(const CVector& position)
{
    assert(pNetGame != nullptr);
    assert(pNetGame->pPlayerPool != nullptr);

    PackGetStruct(&*this->packetStreamUpdatePosition, SV::UpdateLPStreamPositionPacket)->position = position;

    if (pNetGame->pPlayerPool->dwConnectedPlayers != 0)
    {
        const auto playerPoolSize = pNetGame->pPlayerPool->dwPlayerPoolSize;

        for (uint16_t iPlayerId { 0 }; iPlayerId <= playerPoolSize; ++iPlayerId)
        {
            if (this->HasListener(iPlayerId) && PlayerStore::IsPlayerConnected(iPlayerId))
                Network::SendControlPacket(iPlayerId, *&*this->packetStreamUpdatePosition);
        }
    }
}
