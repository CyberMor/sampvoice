/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "PointStream.h"

#include <ysf/globals.h>

#include "Network.h"
#include "PlayerStore.h"
#include "Header.h"

PointStream::PointStream(const float distance, const CVector& position) : LocalStream(distance) {

	if (PackWrap(this->packetStreamUpdatePosition, SV::ControlPacketType::updateLPStreamPosition, sizeof(SV::UpdateLPStreamPositionPacket))) {

		PackGetStruct(&*this->packetStreamUpdatePosition, SV::UpdateLPStreamPositionPacket)->stream = (uint32_t)(static_cast<Stream*>(this));
		PackGetStruct(&*this->packetStreamUpdatePosition, SV::UpdateLPStreamPositionPacket)->position = position;

	}

}

void PointStream::UpdatePosition(const CVector& position) {

	if (!this->packetStreamUpdatePosition) return;

	PackGetStruct(&*this->packetStreamUpdatePosition, SV::UpdateLPStreamPositionPacket)->position = position;

	const auto playerPoolSize = pNetGame->pPlayerPool->dwPlayerPoolSize;
	const auto connectedPlayers = pNetGame->pPlayerPool->dwConnectedPlayers;

	if (connectedPlayers) for (uint16_t iPlayerId = 0; iPlayerId <= playerPoolSize; ++iPlayerId)
		if (this->attachedListeners[iPlayerId].load() && PlayerStore::IsPlayerHasPlugin(iPlayerId))
			Network::SendControlPacket(iPlayerId, *&*this->packetStreamUpdatePosition);

}

PointStream::~PointStream() {}
