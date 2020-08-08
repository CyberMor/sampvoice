/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "DynamicLocalStreamAtVehicle.h"

#include <string.h>

#include <ysf/globals.h>
#include <util/logger.h>
#include <util/memory.hpp>

#include "ControlPacket.h"
#include "PlayerStore.h"
#include "Header.h"

DynamicLocalStreamAtVehicle::DynamicLocalStreamAtVehicle(
	const float distance, const uint32_t maxPlayers,
	const uint16_t vehicleId, const uint32_t color,
	const std::string& name
) :
	LocalStream(distance),
	DynamicStream(distance, maxPlayers)
{

	const auto nameString = name.c_str();
	const auto nameLength = name.size() + 1;

	if (PackWrap(this->packetCreateStream, SV::ControlPacketType::createLStreamAtVehicle, sizeof(SV::CreateLStreamAtPacket) + nameLength)) {

		PackGetStruct(&*this->packetCreateStream, SV::CreateLStreamAtPacket)->stream = (uint32_t)(static_cast<Stream*>(this));
		memcpy(PackGetStruct(&*this->packetCreateStream, SV::CreateLStreamAtPacket)->name, nameString, nameLength);
		PackGetStruct(&*this->packetCreateStream, SV::CreateLStreamAtPacket)->distance = distance;
		PackGetStruct(&*this->packetCreateStream, SV::CreateLStreamAtPacket)->target = vehicleId;
		PackGetStruct(&*this->packetCreateStream, SV::CreateLStreamAtPacket)->color = color;

		if (!pNetGame->pVehiclePool->pVehicle[vehicleId]) return;

		const auto playerPoolSize = pNetGame->pPlayerPool->dwPlayerPoolSize;
		const auto connectedPlayers = pNetGame->pPlayerPool->dwConnectedPlayers;

		const CVector& streamPosition = pNetGame->pVehiclePool->pVehicle[vehicleId]->vecPosition;

		PlayerSortList playerList;

		if (connectedPlayers) for (uint16_t iPlayerId = 0; iPlayerId <= playerPoolSize; ++iPlayerId) {

			if (!PlayerStore::IsPlayerHasPlugin(iPlayerId) || !pNetGame->pPlayerPool->pPlayer[iPlayerId] ||
				!pNetGame->pPlayerPool->pPlayer[iPlayerId]->byteVehicleStreamedIn[vehicleId]) continue;

			const CVector& playerPosition = pNetGame->pPlayerPool->pPlayer[iPlayerId]->vecPosition;
			const float distanceToPlayer = (playerPosition - streamPosition).Length();

			if (distanceToPlayer <= distance) playerList.emplace(distanceToPlayer, iPlayerId);

		}

		for (const auto& playerInfo : playerList) {

			if (this->attachedListenersCount >= maxPlayers) break;

			this->Stream::AttachListener(playerInfo.playerId);

		}

	}

}

void DynamicLocalStreamAtVehicle::Tick() {

	if (!this->packetCreateStream) return;
	if (!this->packetStreamUpdateDistance) return;

	const uint16_t vehicleId = PackGetStruct(&*this->packetCreateStream, SV::CreateLStreamAtPacket)->target;

	if (!pNetGame->pVehiclePool->pVehicle[vehicleId]) return;

	const auto playerPoolSize = pNetGame->pPlayerPool->dwPlayerPoolSize;
	const auto connectedPlayers = pNetGame->pPlayerPool->dwConnectedPlayers;

	const float streamDistance = PackGetStruct(&*this->packetStreamUpdateDistance, SV::UpdateLPStreamDistancePacket)->distance;
	const CVector& streamPosition = pNetGame->pVehiclePool->pVehicle[vehicleId]->vecPosition;

	PlayerSortList playerList;

	if (connectedPlayers) for (uint16_t iPlayerId = 0; iPlayerId <= playerPoolSize; ++iPlayerId) {

		float distanceToPlayer;

		if (PlayerStore::IsPlayerHasPlugin(iPlayerId) && pNetGame->pPlayerPool->pPlayer[iPlayerId] &&
			pNetGame->pPlayerPool->pPlayer[iPlayerId]->byteVehicleStreamedIn[vehicleId] &&
			(distanceToPlayer = (pNetGame->pPlayerPool->pPlayer[iPlayerId]->vecPosition -
				streamPosition).Length()) <= streamDistance
		) {

			if (this->HasListener(iPlayerId)) continue;

			playerList.emplace(distanceToPlayer, iPlayerId);

		}

		else if (this->HasListener(iPlayerId)) {

			this->Stream::DetachListener(iPlayerId);

		}

	}

	for (const auto& playerInfo : playerList) {

		if (this->attachedListenersCount >= this->maxPlayers) break;

		this->Stream::AttachListener(playerInfo.playerId);

	}

}
