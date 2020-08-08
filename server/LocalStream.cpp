/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "LocalStream.h"

#include <ysf/globals.h>

#include "Network.h"
#include "PlayerStore.h"
#include "Header.h"

LocalStream::LocalStream(const float distance) {

	if (PackWrap(this->packetStreamUpdateDistance, SV::ControlPacketType::updateLPStreamDistance, sizeof(SV::UpdateLPStreamDistancePacket))) {

		PackGetStruct(&*this->packetStreamUpdateDistance, SV::UpdateLPStreamDistancePacket)->stream = (uint32_t)(static_cast<Stream*>(this));
		PackGetStruct(&*this->packetStreamUpdateDistance, SV::UpdateLPStreamDistancePacket)->distance = distance;

	}

}

void LocalStream::UpdateDistance(const float distance) {

	if (!this->packetStreamUpdateDistance) return;

	PackGetStruct(&*this->packetStreamUpdateDistance, SV::UpdateLPStreamDistancePacket)->distance = distance;

	const auto playerPoolSize = pNetGame->pPlayerPool->dwPlayerPoolSize;
	const auto connectedPlayers = pNetGame->pPlayerPool->dwConnectedPlayers;

	if (connectedPlayers) for (uint16_t iPlayerId = 0; iPlayerId <= playerPoolSize; ++iPlayerId)
		if (this->attachedListeners[iPlayerId].load() && PlayerStore::IsPlayerHasPlugin(iPlayerId))
			Network::SendControlPacket(iPlayerId, *&*this->packetStreamUpdateDistance);

}

LocalStream::~LocalStream() {}
