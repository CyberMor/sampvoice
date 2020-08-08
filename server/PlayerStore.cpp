/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "PlayerStore.h"

#include <ysf/globals.h>

std::array<std::shared_mutex, MAX_PLAYERS> PlayerStore::playerMutex;
std::array<std::atomic<PlayerInfo*>, MAX_PLAYERS> PlayerStore::playerInfo = {};

void PlayerStore::AddPlayerToStore(const uint16_t playerId, const uint8_t version, const bool microStatus) {

	if (playerId >= MAX_PLAYERS) return;

	if (const auto pPlayerInfo = new PlayerInfo(version, microStatus)) {

		PlayerStore::playerMutex[playerId].lock();
		const auto pOldPlayerInfo = PlayerStore::playerInfo[playerId].exchange(pPlayerInfo);
		PlayerStore::playerMutex[playerId].unlock();

		if (pOldPlayerInfo) {

			for (const auto& pStream : pOldPlayerInfo->listenerStreams)
				pStream->DetachListener(playerId);

			for (const auto& pStream : pOldPlayerInfo->speakerStreams)
				pStream->DetachSpeaker(playerId);

			delete pOldPlayerInfo;

		}

	}

}

void PlayerStore::RemovePlayerFromStore(const uint16_t playerId) {

	if (playerId >= MAX_PLAYERS) return;

	PlayerStore::playerMutex[playerId].lock();
	const auto pPlayerInfo = PlayerStore::playerInfo[playerId].exchange(nullptr);
	PlayerStore::playerMutex[playerId].unlock();

	if (pPlayerInfo) {

		for (const auto& pStream : pPlayerInfo->listenerStreams)
			pStream->DetachListener(playerId);

		for (const auto& pStream : pPlayerInfo->speakerStreams)
			pStream->DetachSpeaker(playerId);

		delete pPlayerInfo;

	}

}

void PlayerStore::ClearStore() {

	for (uint16_t playerId = 0; playerId < MAX_PLAYERS; ++playerId)
		PlayerStore::RemovePlayerFromStore(playerId);

}

bool PlayerStore::IsPlayerConnected(const uint16_t playerId) {

	if (playerId >= MAX_PLAYERS) return false;

	return pNetGame->pPlayerPool->bIsPlayerConnected[playerId];

}

bool PlayerStore::IsPlayerHasPlugin(const uint16_t playerId) {

	if (playerId >= MAX_PLAYERS) return false;

	return PlayerStore::playerInfo[playerId].load();

}

PlayerInfo* PlayerStore::RequestPlayerWithSharedAccess(const uint16_t playerId) {

	if (playerId >= MAX_PLAYERS) return nullptr;

	PlayerStore::playerMutex[playerId].lock_shared();

	return PlayerStore::playerInfo[playerId].load();

}

void PlayerStore::ReleasePlayerWithSharedAccess(const uint16_t playerId) {

	if (playerId >= MAX_PLAYERS) return;

	PlayerStore::playerMutex[playerId].unlock_shared();

}

PlayerInfo* PlayerStore::RequestPlayerWithUniqueAccess(const uint16_t playerId) {

	if (playerId >= MAX_PLAYERS) return nullptr;

	PlayerStore::playerMutex[playerId].lock();

	return PlayerStore::playerInfo[playerId].load();

}

void PlayerStore::ReleasePlayerWithUniqueAccess(const uint16_t playerId) {

	if (playerId >= MAX_PLAYERS) return;

	PlayerStore::playerMutex[playerId].unlock();

}
